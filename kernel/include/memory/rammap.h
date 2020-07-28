#ifndef __RAMMAP_H_INCLUDED__
#define __RAMMAP_H_INCLUDED__

#include <utils.h>

#define RAM_MAPPED_END (16 MB)

void rammapper_map_ram(void);
uint64_t rammapper_get_ram_limit(void) PURE;

#endif