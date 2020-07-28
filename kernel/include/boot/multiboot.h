#ifndef __MULTIBOOT_H_INCLUDED__
#define __MULTIBOOT_H_INCLUDED__

#include <acpi/rsdp.h>
#include <utils.h>

#define MULTIBOOT_TAG_TYPE_END 0
#define MULTIBOOT_TAG_TYPE_MMAP 6
#define MULTIBOOT_TAG_TYPE_RSDPV1 14
#define MULTIBOOT_TAG_TYPE_RSDPV2 15

struct multiboot_mmap_entry {
	uint64_t addr;
	uint64_t len;
#define MULTIBOOT_MMAP_TYPE_AVAILABLE 1
	uint32_t type;
	uint32_t zero;
} PACKED;

struct multiboot_tag_mmap {
	uint32_t type;
	uint32_t size;
	uint32_t entry_size;
	uint32_t entry_version;
	struct multiboot_mmap_entry entries[];
} PACKED;

struct multiboot_tag_rsdpv1 {
	uint32_t type;
	uint32_t size;
	struct rsdpv1 rsdp;
} PACKED;

struct multiboot_tag_rsdpv2 {
	uint32_t type;
	uint32_t size;
	struct rsdpv2 rsdp;
} PACKED;

void multiboot_init(uint64_t multiboot_info_phys);
struct multiboot_tag_mmap *multiboot_get_mmap(void) PURE;
struct multiboot_tag_rsdpv1 *multiboot_get_rsdpv1(void) PURE;
struct multiboot_tag_rsdpv2 *multiboot_get_rsdpv2(void) PURE;
uint64_t multiboot_get_mmap_entries_count(struct multiboot_tag_mmap *mmap) PURE;

#endif