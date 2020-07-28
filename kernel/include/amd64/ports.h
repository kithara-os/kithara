#ifndef __PORTS_H_INCLUDED__
#define __PORTS_H_INCLUDED__

#include <utils.h>

inline static void outb(uint16_t port, uint8_t val) {
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

inline static void outw(uint16_t port, uint16_t val) {
	asm volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

inline static uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

inline static void io_wait(void) {
	asm volatile("outb %%al, $0x80" : : "a"(0));
}

#endif