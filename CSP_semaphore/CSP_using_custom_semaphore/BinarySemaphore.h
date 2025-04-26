#ifndef BINARY_SEMAPHORE_H
#define BINARY_SEMAPHORE_H

#include <iostream>
#include "MutexLock.h"

typedef struct {
    int value; // Binary semaphore value (0 or 1)
    Mutex mutex; // Mutex lock for synchronization
} BinarySemaphore;

void binary_semaphore_init(BinarySemaphore *sem, int initial_value) {
    if (initial_value != 0 && initial_value != 1) {
        fprintf(stderr, "Error: Binary semaphore must be initialized to 0 or 1.\n");
        return;
    }
    sem->value = initial_value;
    mutex_init(&sem->mutex);
}

void binary_semaphore_wait(BinarySemaphore *sem) {
    mutex_lock(&sem->mutex);
    while (sem->value == 0) {
        mutex_unlock(&sem->mutex);
        mutex_lock(&sem->mutex);
    }
    sem->value = 0;
    mutex_unlock(&sem->mutex);
}

void binary_semaphore_signal(BinarySemaphore *sem) {
    mutex_lock(&sem->mutex);
    sem->value = 1;
    mutex_unlock(&sem->mutex);
}

void binary_semaphore_destroy(BinarySemaphore *sem) {
    mutex_destroy(&sem->mutex);
}

#endif // BINARY_SEMAPHORE_H
