#include <iostream>
#include <array>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#include "BinarySemaphore.h"

#define N 3

int t = 0;

Mutex t_mutex;

array<BinarySemaphore, N> ingredients;
array<BinarySemaphore, (1 << N)> targets;

BinarySemaphore next_round;

void* agent(void* arg) {

    while (1) {

        binary_semaphore_wait(&next_round);

        int notUsed = rand() % N;

        printf("Agent: Producing items: [ ");
        for (int i = 0; i < N; i++) {
            if (i != notUsed) {
                cout << i << " ";
                binary_semaphore_signal(&ingredients[i]);
            }
        }
        cout << "]\n";

    }
}

void* f(void* arg) {

    int id = *(int*)arg;

    while (1) {

        binary_semaphore_wait(&ingredients[id]);

        mutex_lock(&t_mutex);
        t = t + (1 << id);
        binary_semaphore_signal(&targets[t]);

        mutex_unlock(&t_mutex);
    
    }
}

void* smoker(void* arg) {

    int id = *(int*)arg;

    while (1) {

        binary_semaphore_wait(&targets[(1 << N) - 1 - (1 << id)]);

        cout << "Smoker " << id << " started smoking..." << endl;

        sleep(1);

        cout << "Smoker " << id << " finished smoking." << endl;

        mutex_lock(&t_mutex);    
        t = 0;
        mutex_unlock(&t_mutex);

        binary_semaphore_signal(&next_round);

    }
}

int main() {
    
    for (int i = 0; i < N; i++) {
        binary_semaphore_init(&ingredients[i], 0);
    }

    for (int i = 0; i < (1 << N); i++) {
        binary_semaphore_init(&targets[i], 0);
    }

    binary_semaphore_init(&next_round, 1);

    pthread_t agent_thread, f_threads[N], smoker_threads[N];

    int smoker_ids[N];
    for (int i = 0; i < N; i++) {
        smoker_ids[i] = i;
    }

    for (int i = 0; i < N; i++) {
        pthread_create(&f_threads[i], NULL, f, &smoker_ids[i]);
    }

    sleep(1);

    for (int i = 0; i < N; i++) {
        pthread_create(&smoker_threads[i], NULL, smoker, &smoker_ids[i]);
    }

    sleep(1);

    pthread_create(&agent_thread, NULL, agent, NULL);

    sleep(1);

    for (int i = 0; i < N; i++)
        pthread_join(f_threads[i], NULL);

    for (int i = 0; i < N; i++)
        pthread_join(smoker_threads[i], NULL);

    pthread_join(agent_thread, NULL);

    return 0;
    
}
