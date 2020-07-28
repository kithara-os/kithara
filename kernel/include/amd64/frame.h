#ifndef __FRAME_H_INCLUDED__
#define __FRAME_H_INCLUDED__

#include <utils.h>

struct frame {
	uint64_t fs, gs, ds, es;
	uint64_t r15, r14;
	uint64_t r13, r12, r11, r10, r9, r8;
	uint64_t rbp, rsi, rdi, rdx, rcx, rbx, rax;
	uint64_t errcode, rip, cs, rflags, rsp, ss;
};

#endif