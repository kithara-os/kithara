#include <boot/multiboot.h>

struct multiboot_tag {
	uint32_t type;
	uint32_t size;
} PACKED;

struct multiboot_head {
	uint32_t total_size;
	uint32_t reserved;
	struct multiboot_tag first[];
} PACKED;

PRIVATE struct multiboot_head *head;

void multiboot_init(uint64_t multiboot_info_phys) {
	head = (struct multiboot_head *)(KERNEL_MAPPING_BASE +
					 multiboot_info_phys);
	if (multiboot_info_phys + head->total_size > KERNEL_DATA_SIZE) {
		panic("[multiboot] Panic: Multiboot header is not accessible");
	}
}

inline static struct multiboot_tag *
multiboot_next(struct multiboot_tag *current) {
	uint64_t addr = (uint64_t)current;
	addr += current->size;
	addr = ALIGN_UP(addr, 8);
	return (struct multiboot_tag *)addr;
}

inline static bool multiboot_is_end(struct multiboot_tag *tag) {
	return ((uint64_t)tag >= ((uint64_t)(head)) + head->total_size);
}

struct multiboot_tag *multiboot_get_by_type(uint32_t type) {
	struct multiboot_tag *current = head->first;
	while (!multiboot_is_end(current)) {
		if (current->type == type) {
			return current;
		}
		current = multiboot_next(current);
	}
	return NULL;
}

struct multiboot_tag_mmap *multiboot_get_mmap(void) {
	return (struct multiboot_tag_mmap *)multiboot_get_by_type(
	    MULTIBOOT_TAG_TYPE_MMAP);
}
struct multiboot_tag_rsdpv1 *multiboot_get_rsdpv1(void) {
	return (struct multiboot_tag_rsdpv1 *)multiboot_get_by_type(
	    MULTIBOOT_TAG_TYPE_RSDPV1);
}
struct multiboot_tag_rsdpv2 *multiboot_get_rsdpv2(void) {
	return (struct multiboot_tag_rsdpv2 *)multiboot_get_by_type(
	    MULTIBOOT_TAG_TYPE_RSDPV2);
}

uint64_t multiboot_get_mmap_entries_count(struct multiboot_tag_mmap *mmap) {
	return (mmap->size - sizeof(struct multiboot_tag_mmap)) /
	       sizeof(struct multiboot_mmap_entry);
}