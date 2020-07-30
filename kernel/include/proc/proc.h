#ifndef __THREAD_H_INCLUDED__
#define __THREAD_H_INCLUDED__

#include <amd64/frame.h>

struct ctx {
	struct frame cpu_state;
};

struct thread {
	struct ctx ctx;
	uint64_t id;
	struct thread *next, *prev;
};

struct proc {
	struct thread *head;
	bool lock_thread_switch;
	uint64_t id, threads_count;
	struct proc *next, *prev;
};

#endif