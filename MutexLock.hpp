#ifndef MUTEX_LOCK_H
#define MUTEX_LOCK_H

#include <atomic>

typedef struct {
    std::atomic_flag lock = ATOMIC_FLAG_INIT; // Initialize atomic_flag
} Mutex;

void mutex_init(Mutex *m) {
    m->lock.clear(); // Clear the atomic_flag
}

void mutex_lock(Mutex *m) {
    while (m->lock.test_and_set(std::memory_order_acquire))
        ; // spin-wait (busy wait)
}

void mutex_unlock(Mutex *m) {
    m->lock.clear(std::memory_order_release);
}

void mutex_destroy(Mutex *m) {
    (void)m; 
}

#endif // MUTEX_LOCK_H