#include <acpi/rxsdt.h>
#include <acpi/sdt.h>
#include <boot/multiboot.h>
#include <memory/bdalloc.h>

struct rsdt {
	struct sdt_header base;
	uint32_t paddrs[];
} PACKED;

struct xsdt {
	struct sdt_header base;
	uint64_t paddrs[];
} PACKED;

PRIVATE struct sdt_header **rxsdt_tables;
PRIVATE uint64_t rxsdt_len;

void rxsdt_init(void) {
	struct multiboot_tag_rsdpv1 *v1 = multiboot_get_rsdpv1();
	struct multiboot_tag_rsdpv2 *v2 = multiboot_get_rsdpv2();
	if (v2 != NULL) {
		printf("[rxsdt] Found XSDT!\n");
		assert(rsdpv2_verify(&(v2->rsdp)),
		       "[rxsdt] Panic: Failed to verify RSDPv2\n");
		struct xsdt *xsdt =
		    (struct xsdt *)(v2->rsdp.xsdt + KERNEL_MAPPING_BASE);
		assert(sdt_verify(&(xsdt->base)),
		       "[rxsdt] Panic: Failed to verify XSDT\n");
		uint64_t entries_count =
		    (xsdt->base.len - sizeof(struct xsdt)) / 8;
		rxsdt_tables = bdalloc_new(entries_count * 8);
		rxsdt_len = entries_count;
		assert(rxsdt_tables != NULL,
		       "[rxsdt] Panic: Failed to allocate space for "
		       "rxsdt_tables\n");
		for (uint64_t i = 0; i < entries_count; ++i) {
			rxsdt_tables[i] =
			    (struct sdt_header *)(xsdt->paddrs[i] +
						  KERNEL_MAPPING_BASE);
			printf("[rxsdt] \"");
			write(rxsdt_tables[i]->sig, 4);
			printf("\" table detected!\n");
		}
	} else {
		printf("[rxsdt] Found RSDT!\n");
		assert(v1 != NULL, "[rxsdt] Panic: Failed to find "
				   "rsdt/xsdt in the multiboot info\n");
		assert(rsdpv1_verify(&(v1->rsdp)),
		       "[rxsdt] Panic: Failed to verify RSDPv1\n");
		struct rsdt *rsdt =
		    (struct rsdt *)(v1->rsdp.rsdt + KERNEL_MAPPING_BASE);
		assert(sdt_verify(&(rsdt->base)),
		       "[rxsdt] Panic: Failed to verify RSDT\n");
		uint64_t entries_count =
		    (rsdt->base.len - sizeof(struct rsdt)) / 4;
		rxsdt_tables = bdalloc_new(entries_count * 8);
		rxsdt_len = entries_count;
		assert(rxsdt_tables != NULL,
		       "[rxsdt] Panic: Failed to allocate space for "
		       "rxsdt_tables\n");
		for (uint64_t i = 0; i < entries_count; ++i) {
			rxsdt_tables[i] =
			    (struct sdt_header *)((uint64_t)(rsdt->paddrs[i]) +
						  KERNEL_MAPPING_BASE);
			printf("[rxsdt] \"");
			write(rxsdt_tables[i]->sig, 4);
			printf("\" table detected!\n");
		}
	}
}

struct sdt_header *rxsdt_query_table(const char *name) {
	for (uint64_t i = 0; i < rxsdt_len; ++i) {
		const char *table_name = rxsdt_tables[i]->sig;
		if (table_name[0] == name[0] && table_name[1] == name[1] &&
		    table_name[2] == name[2] && table_name[3] == name[3]) {
			return rxsdt_tables[i];
		}
	}
	return NULL;
}