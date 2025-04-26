#include <iostream>
#include <array>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#include "BinarySemaphore.hpp"

#define N 10

int t = 0;

Mutex t_mutex; // mutext for t

array<BinarySemaphore, N> ingredients; // semaphores for ingredients
array<BinarySemaphore, (1 << N)> targets; // semaphores for targets

BinarySemaphore next_round; // semaphore for next round

void* agent(void* arg) {

    printf("Agent: Starting...\n\n");

    while (1) {

        binary_semaphore_wait(&next_round); // Wait for the next round

        int notUsed = rand() % N; // Randomly select an item to exclude

        printf("Agent: Producing items: ");
        for (int i = 0; i < N; i++) {
            if (i != notUsed) {
                cout << i << " \n"[(i == N - 1) || (notUsed == N-1 && i == N-2)];
                binary_semaphore_signal(&ingredients[i]);
            }
        }

    }

    return nullptr;
}

void* f(void* arg) {
    int id = *(int*)arg;

    while (1) {

        binary_semaphore_wait(&ingredients[id]); // Wait for the ingredient to be produced

        mutex_lock(&t_mutex);

        t = t + (1 << id); // Update bitmask according to the ingredient
        binary_semaphore_signal(&targets[t]); // Notify if any smoker can smoke
        
        mutex_unlock(&t_mutex);

    }

    return nullptr;
}

void* smoker(void* arg) {

    int id = *(int*)arg;

    while (1) {

        binary_semaphore_wait(&targets[(1 << N) - 1 - (1 << id)]);

        printf("Smoker %d: Smoking...\n", id);
        sleep(1); // simulation of smoking time
        printf("Smoker %d: Finished smoking\n\n", id);

        mutex_lock(&t_mutex);
        t = 0; // Resetting t for the next round
        mutex_unlock(&t_mutex);

        binary_semaphore_signal(&next_round);
    
    }

    return nullptr;
}

int main() {
    
    // Initializing semaphores for ingredients
    for (int i = 0; i < N; i++) {
        binary_semaphore_init(&ingredients[i], 0);
    }

    // Initializing semaphores for targets
    for (int i = 0; i < (1 << N); i++) {
        binary_semaphore_init(&targets[i], 0);
    }

    binary_semaphore_init(&next_round, 1); // Initializing next_round semaphore

    // Creating threads for smokers and agent and f functions
    pthread_t agent_thread, f_threads[N], smoker_threads[N];

    int smoker_ids[N];
    for (int i = 0; i < N; i++) {
        smoker_ids[i] = i;
    }

    for (int i = 0; i < N; i++) {
        pthread_create(&f_threads[i], NULL, f, &smoker_ids[i]);
    }

    sleep(1);
    printf("fthreads created\n");

    for (int i = 0; i < N; i++) {
        pthread_create(&smoker_threads[i], NULL, smoker, &smoker_ids[i]);
    }

    sleep(1);
    printf("smoker threads created\n");

    pthread_create(&agent_thread, NULL, agent, NULL);
    printf("agent thread created\n");

    sleep(1);
    
    // joining threads
    for (int i = 0; i < N; i++) {
        pthread_join(f_threads[i], NULL);
    }
    printf("f threads joined\n");

    for (int i = 0; i < N; i++) {
        pthread_join(smoker_threads[i], NULL);
    }
    printf("smoker threads joined\n");

    pthread_join(agent_thread, NULL);
    printf("agent thread joined\n");

    return 0;

}