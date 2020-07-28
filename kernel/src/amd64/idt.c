#include <amd64/idt.h>
#include <memory/bdalloc.h>
#include <smp/localstorage.h>

struct idt_entry {
	uint16_t addr_low;
	uint16_t selector;
	uint8_t ist : 3;
	uint8_t zeroed1 : 5;
	bool ints_enabled : 1;
	uint8_t ones1 : 3;
	uint8_t zeroed2 : 1;
	uint8_t dpl : 2;
	uint8_t present : 1;
	uint16_t addr_middle;
	uint32_t addr_high;
	uint32_t zeroed3;
} PACKED;

struct idt_pointer {
	uint16_t limit;
	uint64_t base;
} PACKED;

struct idt {
	struct idt_entry entries[256];
	struct idt_pointer pointer;
};

extern void idt_load(struct idt_pointer *pointer);
extern void idt_spurious_irq_handler(void);

void idt_handler(void) {
	panic("Panic: Unhandled interrupt!!!");
}

void idt_init(void) {
	struct idt *idt = (struct idt *)bdalloc_new(sizeof(struct idt));
	assert(idt != NULL, "[idt] Panic: Failed to allocate IDT\n");
	memset(&(idt->entries), sizeof(idt->entries), 0);
	idt->pointer.base = (uint64_t)(&(idt->entries));
	idt->pointer.limit = 0x1000;
	idt_load(&(idt->pointer));
	local_storage_get()->idt = idt;
	asm volatile("mfence" ::: "memory");
	for (uint64_t i = 0; i < 256; ++i) {
		idt_set_gate(i, (uint64_t)idt_handler, 0, false);
	}
	idt_set_gate(39, (uint64_t)idt_spurious_irq_handler, 0, false);
}

void idt_set_gate(uint8_t index, uint64_t addr, uint8_t dpl, bool enable_ints) {
	struct idt *idt = local_storage_get()->idt;
	struct idt_entry *entries = idt->entries;
	entries[index].addr_low = (uint16_t)addr;
	entries[index].addr_middle = (uint16_t)(addr >> 16);
	entries[index].addr_high = (uint64_t)(addr >> 32);
	entries[index].zeroed1 = 0;
	entries[index].zeroed2 = 0;
	entries[index].zeroed3 = 0;
	entries[index].ints_enabled = enable_ints;
	entries[index].ist = 0;
	entries[index].dpl = dpl;
	entries[index].ones1 = 0b111;
	entries[index].selector = 0x8;
	entries[index].present = true;
}