#ifndef __TSS_H_INCLUDED__
#define __TSS_H_INCLUDED__

#include <utils.h>

struct tss {
	uint32_t : 32;
	uint64_t rsp[3];
	uint64_t : 64;
	uint64_t ist[7];
	uint64_t : 64;
	uint16_t : 16;
	uint16_t iopb;
} PACKED;

void tss_init(void);
void tss_load(void);

#endif