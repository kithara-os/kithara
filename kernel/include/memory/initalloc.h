#ifndef __INITALLOC_H_INCLUDED__
#define __INITALLOC_H_INCLUDED__

#include <utils.h>

void initalloc_init(void);
void *initalloc_new(uint64_t size, uint64_t align);

#endif