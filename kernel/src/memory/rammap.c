#include <boot/multiboot.h>
#include <memory/rammap.h>
#include <memory/tmpphys.h>
#include <memory/vmbase.h>

extern uint64_t p4_table;

PRIVATE uint64_t rammapper_p4_phys;
PRIVATE uint64_t rammapper_limit;
PRIVATE uint64_t rammapper_cur_unmappped;

#define RAMMAPPER_VIRTUAL_FLAGS                                                \
	(PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE)

inline static void rammapper_map_near_end(void) {
	uint64_t table = rammapper_p4_phys + KERNEL_MAPPING_BASE;
	uint64_t vaddr = rammapper_cur_unmappped + KERNEL_MAPPING_BASE;
	for (uint64_t i = 0; i < 3; ++i) {
		uint64_t index = vmbase_get_index(vaddr, 4 - i);
		uint64_t *entries = (uint64_t *)table;
		if (entries[index] == 0) {
			entries[index] = tmpphys_get_frame() |
					 RAMMAPPER_VIRTUAL_FLAGS |
					 PAGE_TABLE_USER_ACCESSIBLE;
		}
		table = vmbase_walk_to_index(table, index);
	}
	uint64_t index = vmbase_get_index(vaddr, 1);
	uint64_t *entries = (uint64_t *)table;
	entries[index] = rammapper_cur_unmappped | RAMMAPPER_VIRTUAL_FLAGS;
	vmbase_invalidate_at(vaddr);
}

void rammapper_map_ram(void) {
	rammapper_p4_phys = (uint64_t)(&p4_table) - KERNEL_MAPPING_BASE;
	rammapper_limit = 0;
	struct multiboot_tag_mmap *mmap = multiboot_get_mmap();
	uint64_t entries_count = multiboot_get_mmap_entries_count(mmap);
	for (uint64_t i = 0; i < entries_count; ++i) {
		uint64_t limit = mmap->entries[i].addr + mmap->entries[i].len;
		limit = ALIGN_UP(limit, (4 KB));
		if (limit > rammapper_limit) {
			rammapper_limit = limit;
		}
	}

	rammapper_cur_unmappped = KERNEL_INIT_MAPPING_SIZE;

	while (rammapper_cur_unmappped < rammapper_limit) {
		rammapper_map_near_end();
		rammapper_cur_unmappped += (4 KB);
	}
}

uint64_t rammapper_get_ram_limit(void) {
	return rammapper_limit;
}