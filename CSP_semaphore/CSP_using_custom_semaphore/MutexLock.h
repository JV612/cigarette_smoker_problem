#ifndef MUTEX_LOCK_H
#define MUTEX_LOCK_H

#include <stdatomic.h>

typedef struct {
    atomic_flag lock;
} Mutex;

void mutex_init(Mutex *m) {
    atomic_flag_clear(&m->lock);
}

void mutex_lock(Mutex *m) {
    while (atomic_flag_test_and_set(&m->lock))
        ; // spin-wait (busy wait)
}

void mutex_unlock(Mutex *m) {
    atomic_flag_clear(&m->lock);
}

void mutex_destroy(Mutex *m) {
    // No specific action needed for destruction in this implementation
    // as atomic_flag does not require explicit cleanup.
    (void)m; // Suppress unused parameter warning
}

#endif // MUTEX_LOCK_H