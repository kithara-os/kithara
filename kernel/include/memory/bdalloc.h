#ifndef __BDALLOC_H_INCLUDED__
#define __BDALLOC_H_INCLUDED__

#include <utils.h>

void bdalloc_init(void);
void *bdalloc_new(uint64_t size);
void bdalloc_free(void *loc, uint64_t size);

#endif