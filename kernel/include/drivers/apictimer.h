#ifndef __APICTIMER_H_INCLUDED__
#define __APICTIMER_H_INCLUDED__

#include <amd64/frame.h>

void apic_timer_init(uint64_t ms);

void apic_timer_set_handler(void (*handler)(struct frame *));

#endif