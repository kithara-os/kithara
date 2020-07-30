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
	printf("[smpinit] Local storage initialized on the core %llu!\n",
	       local_storage_get()->id);
	idt_init();
	printf("[smpinit] IDT initialized on core %llu!\n",
	       local_storage_get()->id);
	tss_init();
	printf("[smpinit] TSS initialized on core %llu at %p!\n",
	       local_storage_get()->id, local_storage_get()->tss);
	gdt_init();
	printf("[smpinit] GDT initialized on core %llu!\n",
	       local_storage_get()->id);
	tss_load();
	printf("[smpinit] TSS loaded on core %llu!\n",
	       local_storage_get()->id);
	apic_timer_init(10);
	printf("[smpinit] APIC timer initilaized on the core %llu!\n", local_storage_get()->id);
	uint16_t *flag = (uint16_t *)(0x7ffe + KERNEL_MAPPING_BASE);
	*flag = ~0;
	asm volatile("sti");
	while(true);
}