#ifndef __LOCK_H_INCLUDED__
#define __LOCK_H_INCLUDED__

#include <utils.h>
#include <stdatomic.h>

#define TICKETLOCK_INIT_STATE {0, 0}

struct ticketlock {
	uint64_t cur_ticket;
	uint64_t not_reserved_ticket;
};

inline static void ticketlock_lock(struct ticketlock *lock) {
	uint64_t ticket = atomic_fetch_add(&(lock->not_reserved_ticket), 1);
	while (ticket != atomic_load(&(lock->cur_ticket))) {
		asm volatile("pause");
	}

}
inline static void ticketlock_unlock(struct ticketlock *lock) {
	asm volatile("mfence" ::: "memory");
	atomic_fetch_add(&(lock->cur_ticket), 1);
}

#endif