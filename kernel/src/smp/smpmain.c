#include <amd64/gdt.h>
#include <amd64/idt.h>
#include <amd64/tss.h>
#include <drivers/apictimer.h>
#include <drivers/lapic.h>
#include <smp/localstorage.h>
#include <smp/smp.h>
#include <utils.h>

atomic_ulong cpu_count = 0;

void smpmain() {
	lapic_core_init();
	local_storage_init();
	idt_init();
	tss_init();
	gdt_init();
	tss_load();
	apic_timer_init(10);
	while (atomic_load(&cpu_count) != smp_get_cpu_count()) {
		asm("pause");
	}
	asm volatile("sti" :::);
	printf("[smpinit] GDT, IDT and TSS initialized on core %llu!\n",
	       local_storage_get()->id);
}