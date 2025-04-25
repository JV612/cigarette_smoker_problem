#include <iostream>
#include <array>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <atomic>
#include <mutex>
#include "BinarySemaphore.h"

using namespace std;

#define N 10

int t = 0;

mutex t_mutex;

array<BinarySemaphore, N> ingredients;
array<BinarySemaphore, (1 << N)> targets;

BinarySemaphore next_round;

void* agent(void* arg) {
    printf("Agent: Starting...\n\n");

    while (1) {
        binary_semaphore_wait(&next_round);

        int notUsed = rand() % N;

        printf("Agent: Producing items: [ ");
        for (int i = 0; i < N; i++)
            if (i != notUsed)
                cout << i << " ";
                binary_semaphore_signal(&ingredients[i]);
        
        cout << "]\n";
    }
}

void* f(void* arg) {
    int id = *(int*)arg;

    while (1) {
        binary_semaphore_wait(&ingredients[id]);

        t_mutex.lock();
        t = t + (1 << id);
        binary_semaphore_signal(&targets[t]);
        t_mutex.unlock();
    }
}

void* smoker(void* arg) {
    int id = *(int*)arg;

    while (1) {
        binary_semaphore_wait(&targets[(1 << N) - 1 - (1 << id)]);

        printf("Smoker %d: Smoking...\n", id);
        cout << "Smoker " << id << " is smoking..." << endl;

        sleep(1);

        printf("Smoker %d: Finished smoking\n\n", id);

        t_mutex.lock();
        t = 0;
        t_mutex.unlock();

        binary_semaphore_signal(&next_round);
    }
}

int main() {
    
    for (int i = 0; i < N; i++)
        binary_semaphore_init(&ingredients[i], 0);

    binary_semaphore_init(&next_round, 1);

    pthread_t agent_thread, f_threads[N], smoker_threads[N];

    int smoker_ids[N];
    
    for (int i = 0; i < N; i++)
        smoker_ids[i] = i;

    for (int i = 0; i < N; i++)
        pthread_create(&f_threads[i], NULL, f, &smoker_ids[i]);

    sleep(1);
    printf("fthreads created\n\n");

    for (int i = 0; i < N; i++)
        pthread_create(&smoker_threads[i], NULL, smoker, &smoker_ids[i]);
    

    sleep(1);
    printf("smoker threads created\n\n");

    pthread_create(&agent_thread, NULL, agent, NULL);

    sleep(1);

    for (int i = 0; i < N; i++)
        pthread_join(f_threads[i], NULL);
    
    printf("f threads joined\n");

    for (int i = 0; i < N; i++)
        pthread_join(smoker_threads[i], NULL);
    
    printf("smoker threads joined\n");

    pthread_join(agent_thread, NULL);
    printf("agent thread joined\n");

    return 0;
}
