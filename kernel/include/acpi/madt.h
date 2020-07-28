#ifndef __MADT_H_INCLUDED__
#define __MADT_H_INCLUDED__

#include <acpi/sdt.h>

struct madt_entry {
	uint8_t type;
	uint8_t length;
} PACKED;

struct madt {
	struct sdt_header base;
	uint32_t local_apic;
	uint32_t flags;
	struct madt_entry first_entry[];
} PACKED;

inline static bool madt_inside(struct madt *madt, struct madt_entry *entry) {
	uint64_t end = ((uint64_t)madt) + madt->base.len;
	return ((uint64_t)entry) < end;
}

inline static struct madt_entry *madt_next(struct madt_entry *entry) {
	return (struct madt_entry *)(((uint64_t)entry) + entry->length);
}

#define MADT_ENTRY_LOCAL_APIC 0
struct madt_entry_local_apic {
	uint8_t type;
	uint8_t length;
	uint8_t acpi_processor_id;
	uint8_t apic_id;
	uint32_t flags;
} PACKED;

#define MADT_ENTRY_IOAPIC 1
struct madt_entry_ioapic {
	uint8_t type;
	uint8_t length;
	uint8_t ioapic_id;
	uint8_t reserved;
	uint32_t paddr;
	uint32_t global_sys_int_base;
} PACKED;

#define MADT_ENTRY_INT_SOURCE_OVERRIDE 2
struct madt_entry_int_source_override {
	uint8_t type;
	uint8_t length;
	uint8_t bus_source;
	uint8_t irq_source;
	uint32_t global_sys_interrupt;
	uint16_t flags;
} PACKED;

#define MADT_ENTRY_NMI 4
struct madt_entry_nmi {
	uint8_t type;
	uint8_t length;
	uint8_t acpi_processor_id;
	uint16_t flags;
	uint8_t lint;
} PACKED;

#define MADT_ENTRY_LOCAL_APIC_ADDR_OVERRIDE 5
struct madt_entry_local_apic_addr_override {
	uint8_t type;
	uint8_t length;
	uint16_t reserved;
	uint64_t local_apic_addr;
} PACKED;

void madt_init(void);
uint64_t madt_get_lapic_base(void);
struct madt *madt_get(void);
uint64_t madt_get_cpu_count(void);
uint64_t madt_get_cpu_id(uint64_t apic_id);

#endif