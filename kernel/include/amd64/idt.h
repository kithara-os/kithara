#ifndef __IDT_H_INCLUDED__
#define __IDT_H_INLCUDED__

#include <utils.h>

void idt_init(void);
void idt_set_gate(uint8_t vec, uint64_t addr, uint8_t dpl, bool enable_ints);

#endif