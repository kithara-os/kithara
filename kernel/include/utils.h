#ifndef __UTILS_H_INCLUDED__
#define __UTILS_H_INCLUDED__

#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define KB *1024
#define MB *(1024 KB)
#define GB *(1024 MB)

#define PRIVATE __attribute__((visibility("hidden")))
#define PACKED __attribute__((packed))
#define PURE __attribute__((pure))
#define KERNEL_MAPPING_BASE 0xffff800000000000
#define KERNEL_INIT_MAPPING_SIZE (16 MB)
#define KERNEL_DATA_SIZE (8 MB)
#define TYPEOF __typeof__

#define ALIGN_UP(val, align)                                                   \
	(TYPEOF(val))(((((((uint64_t)(val)) + ((uint64_t)(align)) - 1) /       \
			 ((uint64_t)(align))))) *                              \
		      ((uint64_t)(align)))

#define ALIGN_DOWN(val, align)                                                 \
	(TYPEOF(val))(((uint64_t)(val) / (uint64_t)(align)) * (uint64_t)(align))

inline void memcpy(void *dst, const void *src, uint64_t size) {
	for (uint64_t i = 0; i < size; ++i) {
		((char *)dst)[i] = ((const char *)src)[i];
	}
}

inline void memset(void *dst, uint64_t size, char c) {
	for (uint64_t i = 0; i < size; ++i) {
		((char *)dst)[i] = c;
	}
}

void panic(const char *str);

inline void assert(bool cond, const char *str) {
	if (!cond) {
		panic(str);
	}
}

void printf(const char *fmt, ...);
void write(const char *str, uint64_t size);

#define BITSONES(size) (((1ULL << size) - 1ULL))
#define BITSGET(val, start, size) ((val >> start) & BITSONES(size))
#define BITSSET(val, start, size) (val | (BITSONES(size) << start))
#define BITSCLEAR(val, start, size) (val & ~(BITSONES(size) << start))
#define BITSARESET(val, start, size)                                           \
	(BITSGET(val, start, size) == BITSONES(size))
#define BITSARECLEAR(val, start, size) (BITSGET(val, start, size) == 0)

inline static uint64_t log2(uint64_t value) {
	uint64_t val = 1;
	uint64_t res = 0;
	while (val < value) {
		val *= 2;
		++res;
	}
	return res;
}

#endif