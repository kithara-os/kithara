#ifndef __SMP_H_INCLUDED__
#define __SMP_H_INCLUDED__

#include <utils.h>

void smp_init(void);
uint64_t smp_get_cpu_count(void) PURE;
uint64_t smp_get_stack_used(void) PURE;

#endif