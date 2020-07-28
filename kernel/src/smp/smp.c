#include <acpi/madt.h>
#include <amd64/ports.h>
#include <cpu.h>
#include <drivers/lapic.h>
#include <drivers/pit.h>
#include <memory/bdalloc.h>
#include <smp/smp.h>
#include <smp/smpmain.h>

PRIVATE uint64_t smp_cpu_count;

extern void apboot_load_code(void);

inline static void smp_cpu_start(uint64_t id) {
	// flag that trampoline code needs to set when it is ready
	uint16_t *flag = (uint16_t *)(0x7ffe + KERNEL_MAPPING_BASE);
	// we already have a cool init cr3, so why bother creating a new one
	// let's pass it to cpu via memory, so it can read it as dword [0x7ff8]
	*(uint32_t *)(0x7ff8 + KERNEL_MAPPING_BASE) = (uint32_t)(cpu_get_cr3());
	// a method trampoline code should call when it is in 64 bit mode
	*(uint64_t *)(0x7ff0 + KERNEL_MAPPING_BASE) = (uint64_t)(smpmain);
	// stack for cpu to use
	void *stack = bdalloc_new(0x10000);
	if (stack == NULL) {
		panic("[smp] Panic: Failed to allocate CPU stack\n");
	}
	*(uint64_t *)(0x7fe8 + KERNEL_MAPPING_BASE) =
	    ((uint64_t)stack + (0x10000));
	// just to be sure
	asm volatile("mfence" : : : "memory");
	// copying init code to physical ram at 0x8000
	extern uint64_t apcode_start, apcode_end;
	uint64_t *apcode_startp = (uint64_t *)&apcode_start;
	uint64_t apcode_size =
	    (uint64_t)(&apcode_end) - (uint64_t)(&apcode_start);
	memcpy((uint64_t *)(KERNEL_MAPPING_BASE + 0x8000), apcode_startp,
	       apcode_size);
	// initial flag value
	*flag = 0;
	// start cpu. Method: https://wiki.osdev.org/SMP
	lapic_send_ipi_sequence(id);
	pit_wait(10);
	lapic_send_sipi_sequence(id, 0x8000);
	pit_wait(1);
	if (*flag != 0) {
		return;
	}
	lapic_send_sipi_sequence(id, 0x8000);
	for (uint64_t i = 0; i < 100; ++i) {
		pit_wait(10);
		if (*flag != 0) {
			return;
		}
	}
	printf("[smp] Panic: CPU with id %llu is not responging\n", id);
	panic("");
}

void smp_init(void) {
	struct madt *madt = madt_get();
	struct madt_entry *entry = madt->first_entry;
	uint64_t my_id = lapic_get_apic_id();
	smp_cpu_count = 0;
	while (madt_inside(madt, entry)) {
		if (entry->type == MADT_ENTRY_LOCAL_APIC) {
			struct madt_entry_local_apic *cpu_entry =
			    (struct madt_entry_local_apic *)entry;
			smp_cpu_count++;
			if (cpu_entry->apic_id != my_id) {
				smp_cpu_start(cpu_entry->apic_id);
			}
		}
		entry = madt_next(entry);
	}
}

uint64_t smp_get_cpu_count(void) {
	return smp_cpu_count;
}