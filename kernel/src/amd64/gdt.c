#include <amd64/gdt.h>
#include <amd64/tss.h>
#include <memory/bdalloc.h>
#include <smp/localstorage.h>

#define GDT_ENTRIES 7

struct gdt {
	uint64_t entries[GDT_ENTRIES];
	struct gdt_pointer {
		uint16_t limit;
		uint64_t base;
	} PACKED pointer;
};

extern void gdt_load(struct gdt_pointer *pointer);

void gdt_init(void) {
	struct gdt *gdt = bdalloc_new(sizeof(struct gdt));
	assert(gdt != NULL, "[gdt] Panic: Failed to allocated GDT\n");
	gdt->entries[0] = 0;
	gdt->entries[1] = (1LLU << 44) | (1LLU << 47) | (1LLU << 41LLU) |
			  (1LLU << 43) | (1LLU << 53);
	gdt->entries[2] = (1LLU << 44) | (1LLU << 47) | (1LLU << 41LLU);
	gdt->entries[3] = (1LLU << 44) | (1LLU << 47) | (1LLU << 41LLU) |
			  (1LLU << 43) | (1LLU << 53) | (1LLU << 46) |
			  (1LLU << 45);
	gdt->entries[4] = (1LLU << 44) | (1LLU << 47) | (1LLU << 41LLU) |
			  (1LLU << 46) | (1LLU << 45);
	uint64_t tss_base = (uint64_t)(local_storage_get()->tss);
	gdt->entries[5] = ((sizeof(struct tss) - 1) & 0xffff) |
			  ((tss_base & 0xffffff) << 16) | (0b1001LLU << 40) |
			  (1LLU << 47) | (((tss_base >> 24) & 0xff) << 56);
	gdt->entries[6] = tss_base >> 32;
	gdt->pointer.base = (uint64_t)(&(gdt->entries));
	gdt->pointer.limit = 8 * GDT_ENTRIES - 1;
	gdt_load(&(gdt->pointer));
}