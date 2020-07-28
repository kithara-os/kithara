#include <acpi/madt.h>
#include <acpi/rxsdt.h>

PRIVATE struct madt *madt;
PRIVATE uint64_t madt_cpu_count;

void madt_init(void) {
	madt = (struct madt *)rxsdt_query_table("APIC");
	assert(madt != NULL, "[madt] \"APIC\" table not found\n");
	assert(sdt_verify(&(madt->base)),
	       "[madt] Failed to verify MADT table\n");
	struct madt_entry *entry = madt->first_entry;
	madt_cpu_count = 0;
	while (madt_inside(madt, entry)) {
		if (entry->type == MADT_ENTRY_LOCAL_APIC) {
			madt_cpu_count++;
		}
		entry = madt_next(entry);
	}
}

struct madt *madt_get(void) {
	return madt;
}

uint64_t madt_get_lapic_base(void) {
	uint64_t result = (uint64_t)(madt->local_apic);
	struct madt_entry *entry = madt->first_entry;

	while (madt_inside(madt, entry)) {
		if (entry->type == MADT_ENTRY_LOCAL_APIC_ADDR_OVERRIDE) {
			struct madt_entry_local_apic_addr_override *override =
			    (struct madt_entry_local_apic_addr_override *)entry;
			result = override->local_apic_addr;
			return result;
		}
		entry = madt_next(entry);
	}
	return result;
}

uint64_t madt_get_cpu_count(void) {
	return madt_cpu_count;
}

uint64_t madt_get_cpu_id(uint64_t apic_id) {
	struct madt_entry *entry = madt->first_entry;
	while (madt_inside(madt, entry)) {
		if (entry->type == MADT_ENTRY_LOCAL_APIC) {
			struct madt_entry_local_apic *cpu_entry =
			    (struct madt_entry_local_apic *)entry;
			if (cpu_entry->apic_id == apic_id) {
				return cpu_entry->acpi_processor_id;
			}
		}
		entry = madt_next(entry);
	}
	panic("[madt] id of non existing CPU was queried");
	return (uint64_t)(-1);
}