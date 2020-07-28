#include <acpi/madt.h>
#include <cpu.h>
#include <drivers/lapic.h>

// from https://wiki.osdev.org/APIC
#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800

PRIVATE uint64_t lapic_base;

inline static bool lapic_check_presence(void) {
	uint32_t eax, edx;
	cpu_cpuid(1, &eax, &edx);
	return (edx & (1ULL << 9)) != 0;
}

void lapic_init(void) {
	assert(lapic_check_presence(), "[lapic] lapic is not present");
	lapic_base = madt_get_lapic_base();
	lapic_core_init();
}

void lapic_core_init(void) {
	cpu_wrmsr(IA32_APIC_BASE_MSR,
		  cpu_rdmsr(IA32_APIC_BASE_MSR) | IA32_APIC_BASE_MSR_ENABLE);
	lapic_write(LAPIC_REG_SPURIOUS, lapic_read(0xf0) | 0x100);
}

void lapic_write(uint64_t reg, uint32_t val) {
	*((uint32_t volatile *)(lapic_base + reg + KERNEL_MAPPING_BASE)) = val;
}

uint32_t lapic_read(uint64_t reg) {
	return *((uint32_t volatile *)(lapic_base + reg + KERNEL_MAPPING_BASE));
}

void lapic_send_ipi_sequence(uint32_t id) {
	while (true) {
		lapic_write(LAPIC_REG_ICRH, (id << 24));
		lapic_write(LAPIC_REG_ICRL, 0x4500);
		uint32_t val = lapic_read(0x300);
		if ((val & (1 << 12)) == 0) {
			return;
		}
	}
}

void lapic_send_sipi_sequence(uint32_t id, uint32_t entrypoint) {
	uint32_t page_number = entrypoint / 0x1000;
	while (true) {
		lapic_write(0x310, (id << 24));
		lapic_write(0x300, 0x4600 | page_number);
		uint32_t val = lapic_read(0x300);
		if ((val & (1 << 12)) == 0) {
			return;
		}
	}
}

uint32_t lapic_get_apic_id() {
	return (lapic_read(0x20) >> 24);
}