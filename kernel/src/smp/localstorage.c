#include <acpi/madt.h>
#include <cpu.h>
#include <drivers/lapic.h>
#include <memory/bdalloc.h>
#include <smp/localstorage.h>

#define ia32_msr_fs_base 0xC0000100

void local_storage_init(void) {
	struct local_storage *area =
	    (struct local_storage *)bdalloc_new(sizeof(struct local_storage));

	area->self = area;
	area->id = madt_get_cpu_id(lapic_get_apic_id());
	cpu_wrmsr(ia32_msr_fs_base, (uint64_t)area);
}

struct local_storage *local_storage_get(void) {
	return (struct local_storage *)(cpu_rdmsr(ia32_msr_fs_base));
}

void local_storage_set(struct local_storage *storage) {
	cpu_wrmsr(ia32_msr_fs_base, (uint64_t)storage);
}