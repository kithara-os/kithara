#include <memory/initalloc.h>
#include <memory/rammap.h>
#include <memory/tmpphys.h>
#include <memory/vmbase.h>

PRIVATE uint64_t initalloc_brk;
PRIVATE uint64_t initalloc_last_used;
extern uint64_t p4_table;

inline static void initalloc_mapat(uint64_t vaddr) {
	uint64_t table = (uint64_t)&p4_table;
	for (uint64_t i = 0; i < 3; ++i) {
		uint64_t index = vmbase_get_index(vaddr, 4 - i);
		uint64_t *entries = (uint64_t *)table;
		if (entries[index] == 0) {
			entries[index] = tmpphys_get_frame() |
					 PAGE_TABLE_ENTRY_PRESENT |
					 PAGE_TABLE_ENTRY_WRITABLE |
					 PAGE_TABLE_USER_ACCESSIBLE;
		}
		table = vmbase_walk_to_index(table, index);
	}
	uint64_t index = vmbase_get_index(vaddr, 1);
	uint64_t *entries = (uint64_t *)table;
	entries[index] = tmpphys_get_frame() | PAGE_TABLE_ENTRY_PRESENT |
			 PAGE_TABLE_ENTRY_WRITABLE;
	vmbase_invalidate_at(vaddr);
}

void initalloc_init(void) {
	initalloc_brk = rammapper_get_ram_limit() + KERNEL_MAPPING_BASE;
	initalloc_last_used = initalloc_brk;
}

void *initalloc_new(uint64_t size, uint64_t align) {
	uint64_t result = ALIGN_UP(initalloc_last_used, align);
	uint64_t new_end = result + size;
	uint64_t new_brk = ALIGN_UP(new_end, 0x1000);
	for (uint64_t addr = initalloc_brk; addr < new_brk; addr += 0x1000) {
		initalloc_mapat(addr);
	}
	initalloc_last_used = new_end;
	initalloc_brk = new_brk;
	return (void *)result;
}