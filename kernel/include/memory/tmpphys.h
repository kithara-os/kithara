#ifndef __TMP_PHYS_H_INCLUDED__
#define __TMP_PHYS_H_INCLUDED__

#include <utils.h>

void tmpphys_init(void);
uint64_t tmpphys_get_frame(void);
uint64_t tmpphys_get_brk(void) PURE;

#endif