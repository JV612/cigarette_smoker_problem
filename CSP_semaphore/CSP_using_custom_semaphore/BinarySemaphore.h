#ifndef BINARY_SEMAPHORE_H
#define BINARY_SEMAPHORE_H

#include "MutexLock.h"
#include <stdio.h>

typedef struct {
    int value; // Binary semaphore value (0 or 1)
    Mutex mutex; // Mutex lock for synchronization
} BinarySemaphore;

// Initialize the binary semaphore
void binary_semaphore_init(BinarySemaphore *sem, int initial_value) {
    if (initial_value != 0 && initial_value != 1) {
        fprintf(stderr, "Error: Binary semaphore must be initialized to 0 or 1.\n");
        return;
    }
    sem->value = initial_value;
    mutex_init(&sem->mutex);
}

// Wait (P operation) on the semaphore
void binary_semaphore_wait(BinarySemaphore *sem) {
    mutex_lock(&sem->mutex);
    while (sem->value == 0) {
        // Busy wait (can be replaced with condition variables for efficiency)
        mutex_unlock(&sem->mutex);
        mutex_lock(&sem->mutex);
    }
    sem->value = 0;
    mutex_unlock(&sem->mutex);
}

// Try to wait (P operation) on the semaphore without blocking
int binary_semaphore_try_wait(BinarySemaphore *sem) {
    mutex_lock(&sem->mutex);
    if (sem->value == 1) {
        sem->value = 0;
        mutex_unlock(&sem->mutex);
        return 0; // Successfully acquired the semaphore
    } else {
        // Semaphore is already taken, do not block
        mutex_unlock(&sem->mutex);
        return -1;
    }
}

// Signal (V operation) on the semaphore
void binary_semaphore_signal(BinarySemaphore *sem) {
    mutex_lock(&sem->mutex);
    sem->value = 1;
    mutex_unlock(&sem->mutex);
}

// Destroy the binary semaphore
void binary_semaphore_destroy(BinarySemaphore *sem) {
    mutex_destroy(&sem->mutex);
}

#endif // BINARY_SEMAPHORE_H