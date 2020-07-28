#ifndef __VMBASE_H_INCLUDED__
#define __VMBASE_H_INCLUDED__

#include <utils.h>

#define PAGE_TABLE_ENTRY_FLAGS_MASK (0b111111111111ULL | (1ULL << 63))
#define PAGE_TABLE_ENTRY_PRESENT (1ULL << 0)
#define PAGE_TABLE_ENTRY_WRITABLE (1ULL << 1)
#define PAGE_TABLE_USER_ACCESSIBLE (1ULL << 2)

inline static uint64_t vmbase_get_index(uint64_t addr, uint64_t index) {
	return (addr >> (39ULL - (9ULL * (4 - index)))) & 0777ULL;
}

inline static uint64_t vmbase_walk_to_index(uint64_t addr, uint64_t index) {
	uint64_t entry = ((uint64_t *)addr)[index];
	entry &= (~PAGE_TABLE_ENTRY_FLAGS_MASK);
	entry += KERNEL_MAPPING_BASE;
	return entry;
}

void vmbase_invalidate_at(uint64_t addr);
void vmbase_invalidate_all(void);
void vmbase_load_page_table(uint64_t addr);

#endif