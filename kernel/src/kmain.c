#include <acpi/madt.h>
#include <acpi/rxsdt.h>
#include <boot/multiboot.h>
#include <cpu.h>
#include <drivers/lapic.h>
#include <drivers/pic.h>
#include <memory/bdalloc.h>
#include <memory/initalloc.h>
#include <memory/rammap.h>
#include <memory/tmpphys.h>
#include <memory/vmbase.h>
#include <smp/smp.h>
#include <smp/smpmain.h>
#include <utils.h>
#include <video/video.h>

#define INIT_TEMPLATE "[init] Module \"%s\" initalized successfully!\n"

void kmain(uint64_t multiboot_base_phys) {

	video_init();
	printf(INIT_TEMPLATE, "Video Driver");

	multiboot_init(multiboot_base_phys);
	printf(INIT_TEMPLATE, "Multiboot Parser");

	tmpphys_init();
	printf(INIT_TEMPLATE, "Temporatory Physical Allocator");

	rammapper_map_ram();
	printf(INIT_TEMPLATE, "Physical RAM window");

	initalloc_init();
	printf(INIT_TEMPLATE, "Init Virtual Allocator");

	bdalloc_init();
	printf(INIT_TEMPLATE, "Buddy Allocator");

	rxsdt_init();
	printf(INIT_TEMPLATE, "RSDT/XSDT Table Parser");

	madt_init();
	printf(INIT_TEMPLATE, "MADT Table Parser");

	pic_disable();
	printf("[init] 8259 PIC successfully disabled!\n");

	lapic_init();
	printf(INIT_TEMPLATE, "LAPIC Driver");

	smp_init();
	printf(INIT_TEMPLATE, "SMP Startup");

	smpmain();

	while (true) {
		asm volatile("hlt");
	}
}