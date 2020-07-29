#include <boot/multiboot.h>
#include <memory/bdalloc.h>
#include <memory/initalloc.h>
#include <memory/rammap.h>
#include <memory/tmpphys.h>
#include <lock.h>

struct bdalloc_header {
	struct bdalloc_header *next;
	struct bdalloc_header *prev;
};

PRIVATE uint64_t bdalloc_phys_mem_order;
PRIVATE uint64_t *bdalloc_bitmaps[64];
PRIVATE struct bdalloc_header *bdalloc_lists[64];
PRIVATE struct ticketlock bdalloc_lock = TICKETLOCK_INIT_STATE;

inline static struct bdalloc_header *bdalloc_from_phys(uint64_t addr) {
	return (struct bdalloc_header *)(addr + KERNEL_MAPPING_BASE);
}

inline static uint64_t bdalloc_to_phys(struct bdalloc_header *header) {
	return ((uint64_t)header) - KERNEL_MAPPING_BASE;
}

inline static struct bdalloc_header *bdalloc_get_order(uint64_t order) {
	struct bdalloc_header *head = bdalloc_lists[order];
	if (head == NULL) {
		return NULL;
	}
	bdalloc_lists[order] = head->next;
	if (bdalloc_lists[order] != NULL) {
		bdalloc_lists[order]->prev = NULL;
	}
	return head;
}

inline static void bdalloc_insert_order(struct bdalloc_header *header,
					uint64_t order) {
	header->next = bdalloc_lists[order];
	header->prev = NULL;
	if (bdalloc_lists[order] != NULL) {
		bdalloc_lists[order]->prev = header;
	}
	bdalloc_lists[order] = header;
}

inline static void bdalloc_cut_from_list(struct bdalloc_header *header,
					 uint64_t order) {
	struct bdalloc_header *prev, *next;
	prev = header->prev;
	next = header->next;
	if (prev == NULL) {
		bdalloc_lists[order] = next;
	} else {
		prev->next = next;
	}
	if (next != NULL) {
		next->prev = NULL;
	}
}

inline static void bdalloc_set_bit(uint64_t addr, uint64_t order) {
	uint64_t bitmap_index = addr / (1ULL << order);
	bdalloc_bitmaps[order][bitmap_index / 64] |=
	    (1ULL << (bitmap_index % 64));
}

inline static void bdalloc_clear_bit(uint64_t addr, uint64_t order) {
	uint64_t bitmap_index = addr / (1ULL << order);
	bdalloc_bitmaps[order][bitmap_index / 64] &=
	    ~(1ULL << (bitmap_index % 64));
}

inline static void bdalloc_flip_bit(uint64_t addr, uint64_t order) {
	uint64_t bitmap_index = addr / (1ULL << order);
	bdalloc_bitmaps[order][bitmap_index / 64] ^=
	    (1ULL << (bitmap_index % 64));
}

inline static bool bdalloc_get_bit(uint64_t addr, uint64_t order) {
	uint64_t bitmap_index = addr / (1ULL << order);
	return ((bdalloc_bitmaps[order][bitmap_index / 64] &
		 (1ULL << (bitmap_index % 64)))) != 0;
}

inline static void bdalloc_split(struct bdalloc_header *header,
				 uint64_t order) {
	uint64_t left_addr = bdalloc_to_phys(header);
	uint64_t right_addr = left_addr | (1ULL << (order - 1));
	bdalloc_set_bit(left_addr, order);
	bdalloc_insert_order(bdalloc_from_phys(right_addr), order - 1);
}

inline static struct bdalloc_header *
bdalloc_try_to_merge(struct bdalloc_header *header, uint64_t order) {
	uint64_t addr = bdalloc_to_phys(header);
	uint64_t left_addr = addr & ~(1ULL << order);
	if (!bdalloc_get_bit(left_addr, order + 1)) {
		bdalloc_set_bit(left_addr, order + 1);
		return NULL;
	}
	uint64_t other_addr = addr ^ (1ULL << order);
	bdalloc_cut_from_list(bdalloc_from_phys(other_addr), order);
	bdalloc_clear_bit(left_addr, order + 1);
	return bdalloc_from_phys(left_addr);
}

inline static uint64_t bdalloc_max_border(uint64_t current, uint64_t end) {
	uint64_t test_order = 4;
	uint64_t new_order = 4;
	while (ALIGN_DOWN(current, 1ULL << new_order) == current &&
	       (current + (1ULL << new_order)) <= end) {
		test_order = new_order;
		new_order++;
	}
	return test_order;
}

// start and end are physicall addresses
inline static void bdalloc_add_area(uint64_t start, uint64_t end) {
	while (start < end) {
		uint64_t order = bdalloc_max_border(start, end);
		bdalloc_insert_order(bdalloc_from_phys(start), order);
		start += (1ULL << order);
	}
}

inline static void bdalloc_initialize_bitmap(void) {
	for (uint64_t order = 4; order < bdalloc_phys_mem_order; ++order) {
		struct bdalloc_header *head = bdalloc_lists[order];
		while (head != NULL) {
			uint64_t left_addr =
			    bdalloc_to_phys(head) & ~(1ULL << (order));
			bdalloc_set_bit(left_addr, order + 1);
			head = head->next;
		}
	}
}

inline static void bdalloc_traverse(void) {
	for (uint64_t order = 4; order < bdalloc_phys_mem_order; ++order) {
		printf("%llu: ", order);
		struct bdalloc_header *head = bdalloc_lists[order];
		while (head != NULL) {
			printf("|");
			head = head->next;
		}
		printf(" ");
	}
	printf("\n");
}

void bdalloc_init(void) {
	for (uint64_t i = 0; i < 64; ++i) {
		bdalloc_bitmaps[i] = 0;
		bdalloc_lists[i] = NULL;
	}
	bdalloc_phys_mem_order = log2(rammapper_get_ram_limit());
	uint64_t upper_bound = 1ULL << bdalloc_phys_mem_order;
	// minimal block size: 16
	for (uint64_t i = 4; i < bdalloc_phys_mem_order; ++i) {
		uint64_t block_size = 1ULL << i;
		uint64_t count = upper_bound / block_size;
		count = ALIGN_UP(count, 64);
		uint64_t bytesize = count / 8;
		bdalloc_bitmaps[i] = (uint64_t *)initalloc_new(bytesize, 8);
		memset(bdalloc_bitmaps[i], bytesize, 0);
	}
	struct multiboot_tag_mmap *mmap = multiboot_get_mmap();
	assert(mmap != NULL, "[bdalloc] panic: no memory map found\n");
	uint64_t count = multiboot_get_mmap_entries_count(mmap);
	for (uint64_t i = 0; i < count; ++i) {
		uint64_t start = ALIGN_DOWN(mmap->entries[i].addr, 0x1000);
		uint64_t end = ALIGN_UP(
		    mmap->entries[i].addr + mmap->entries[i].len, 0x1000);
		if (start < tmpphys_get_brk()) {
			start = tmpphys_get_brk();
		}
		if (start >= end) {
			continue;
		}
		if (mmap->entries[i].type != MULTIBOOT_MMAP_TYPE_AVAILABLE) {
			continue;
		}
		bdalloc_add_area(start, end);
	}
	bdalloc_initialize_bitmap();
	// bdalloc_traverse();
}

void *bdalloc_new(uint64_t size) {
	if (size < 16) {
		size = 16;
	}
	uint64_t order = log2(size);
	if (order >= (bdalloc_phys_mem_order - 1)) {
		return NULL;
	}
	ticketlock_lock(&bdalloc_lock);
	for (uint64_t cur_order = order; cur_order < bdalloc_phys_mem_order;
	     ++cur_order) {
		struct bdalloc_header *header = bdalloc_get_order(cur_order);
		if (header == NULL) {
			continue;
		}

		bdalloc_clear_bit(bdalloc_to_phys(header) &
				      ~(1ULL << (cur_order)),
				  cur_order + 1);
		for (uint64_t i = cur_order; i > order; --i) {
			bdalloc_split(header, i);
		}
		bdalloc_clear_bit(bdalloc_to_phys(header), order + 1);
		// bdalloc_traverse();
		ticketlock_unlock(&bdalloc_lock);
		return (void *)header;
	}
	ticketlock_unlock(&bdalloc_lock);
	return NULL;
}

void bdalloc_free(void *loc, uint64_t size) {
	if (loc == NULL) {
		return;
	}
	ticketlock_lock(&bdalloc_lock);
	assert((uint64_t)loc > KERNEL_MAPPING_BASE,
	       "[bdalloc] area free under KERNEL_MAPPING_BASE\n");
	if (size < 16) {
		size = 16;
	}
	uint64_t order = log2(size);
	struct bdalloc_header *head = (struct bdalloc_header *)loc;
	for (uint64_t i = order; i < bdalloc_phys_mem_order; ++i) {
		struct bdalloc_header *tmp;
		if ((tmp = bdalloc_try_to_merge(head, i)) == NULL) {
			bdalloc_insert_order(head, i);
			ticketlock_unlock(&bdalloc_lock);
			return;
		}
		head = tmp;
	}
	bdalloc_insert_order(head, bdalloc_phys_mem_order);
	ticketlock_unlock(&bdalloc_lock);
}