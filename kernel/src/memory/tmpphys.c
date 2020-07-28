#include <boot/multiboot.h>
#include <memory/tmpphys.h>

PRIVATE uint64_t tmpphys_frame;
PRIVATE struct multiboot_tag_mmap *mmap;
PRIVATE uint64_t tmpphys_area_ind;
extern uint64_t kernel_end;

inline static void tmpphys_update_area(void) {
	while (tmpphys_area_ind < multiboot_get_mmap_entries_count(mmap)) {
		uint64_t area_start = mmap->entries[tmpphys_area_ind].addr;
		uint64_t area_end =
		    area_start + mmap->entries[tmpphys_area_ind].len;
		area_start = ALIGN_UP(area_start, 4 KB);
		area_end = ALIGN_DOWN(area_end, 4 KB);
		if (tmpphys_frame < area_start) {
			tmpphys_frame = area_start;
		}
		if (mmap->entries[tmpphys_area_ind].type !=
		    MULTIBOOT_MMAP_TYPE_AVAILABLE) {
			tmpphys_area_ind++;
			continue;
		}
		if (tmpphys_frame < area_end) {
			return;
		}
		tmpphys_area_ind++;
	}
	panic("[tmpphys] Panic: Failed to allocate enough memory for init");
}

inline static void tmpphys_move_to_next(void) {
	tmpphys_frame += 4 KB;
	tmpphys_update_area();
}

void tmpphys_init(void) {
	mmap = multiboot_get_mmap();
	if (mmap == NULL) {
		panic("[tmpphys] Panic: No memory map found");
	}
	tmpphys_frame = KERNEL_DATA_SIZE;
	if (tmpphys_frame < (uint64_t)(&kernel_end)) {
		tmpphys_frame = (uint64_t)(&kernel_end);
	}
	tmpphys_update_area();
}

uint64_t tmpphys_get_frame(void) {
	uint64_t result = tmpphys_frame;
	tmpphys_move_to_next();
	return result;
}

uint64_t tmpphys_get_brk(void) {
	return tmpphys_frame;
}