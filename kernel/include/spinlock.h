#ifndef __SPINLOCK_H_INCLUDED__
#define __SPINLOCK_H_INCLUDED__

#include <utils.h>

typedef unsigned long spinlock_t;

inline static void spinlock_lock(spinlock_t *lock) {
	while (!__sync_bool_compare_and_swap(lock, 0, 1)) {
		asm("pause");
	}
}

inline static bool spinlock_trylock(spinlock_t *lock) {
	return __sync_bool_compare_and_swap(lock, 0, 1);
}

inline static void spinlock_unlock(spinlock_t *lock) {
	asm volatile("mfence" ::: "memory");
	__sync_bool_compare_and_swap(lock, 1, 0);
}

#endif