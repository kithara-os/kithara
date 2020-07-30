#ifndef __CPU_H_INCLUDED__
#define __CPU_H_INCLUDED__

#include <utils.h>

// from https://wiki.osdev.org/Inline_Assembly/Examples

inline static uint64_t cpu_rdtsc(void) {
	uint32_t low, high;
	asm volatile("rdtsc" : "=a"(low), "=d"(high));
	return ((uint64_t)high << 32) | low;
}

inline static uint64_t cpu_get_cr3(void) {
	uint64_t val;
	asm volatile("mov %%cr3, %0" : "=r"(val));
	return val;
}

inline static void cpu_cpuid(int code, uint32_t *a, uint32_t *d) {
	asm volatile("cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx");
}

inline static uint64_t cpu_rdmsr(uint64_t msr) {
	uint32_t low, high;
	asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
	return ((uint64_t)high << 32) | low;
}

inline static void cpu_wrmsr(uint64_t msr, uint64_t value) {
	uint32_t low = value & 0xFFFFFFFF;
	uint32_t high = value >> 32;
	asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

#endif