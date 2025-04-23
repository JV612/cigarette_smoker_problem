#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 3

int r[N]; // Resource counters

const char* resources[N] = {"TOBACCO", "PAPER", "MATCHES"}; // Resource name

sem_t resource_sem[N]; // Semaphores for resources
sem_t next_round;

void* agent(void* arg) {
    printf("Agent: Starting...\n\n");

    while (1) {
        sem_wait(&next_round); // Wait for the next round to start

        int items[N - 1];

        int notUsed = rand() % N; // Randomly select one item to not use
        for (int i = 0; i < N; i++)
            if (i != notUsed)
                items[i < notUsed ? i : i - 1] = i;

        printf("Agent: Producing items: ");
        for (int i = 0; i < N - 1; i++)
            printf("%s ", resources[items[i]]); // Print the produced items
        printf("\n");

        for (int i = 0; i < N - 1; i++)
            sem_post(&resource_sem[items[i]]); // Signal the two items
    }
}

void* f(void* arg) {
    int type = *(int*)arg;
    while (1) {
        sem_wait(&resource_sem[type]); // Wait for tobacco

        for (int i = 0; i < N; i++)
            if (i != type)
                r[i]++; // Increment the resource counter for the other items
    }
}

void* smoker(void* arg) {
    int id = *(int*)arg; // Get the smoker ID

    printf("Smoker %d: Starting...\n", id);

    while (1) {
        while (r[id] != 2)
            ; // Wait until the smoker has 2 items

        printf("Smoker %d: Smoking...\n", id);
        sleep(1); // Simulate smoking time
        printf("Smoker %d: Finished smoking\n\n", id);

        for (int i = 0; i < N; i++)
            r[i] = 0; // Reset the resource counters
        sem_post(&next_round); // Signal the agent to produce more items
    }
}

int main() {
    for (int i = 0; i < N; i++)
        r[i] = 0; // Initialize resource counters
    
    for (int i = 0; i < N; i++)
        sem_init(&resource_sem[i], 0, 0); // Initialize semaphores for resources
    sem_init(&next_round, 0, 1); // Initialize semaphore for next round

    pthread_t agent_thread, f_threads[N], smoker_threads[N];
    
    int smoker_ids[N]; // Array to hold smoker IDs
    for (int i = 0; i < N; i++)
        smoker_ids[i] = i; // Initialize smoker IDs

    for (int i = 0; i < N; i++)
        pthread_create(&f_threads[i], NULL, f, &smoker_ids[i]); // Create smoker threads

    sleep(1);
    printf("fthreads created\n\n");

    for (int i = 0; i < N; i++)
        pthread_create(&smoker_threads[i], NULL, smoker, &smoker_ids[i]); // Create smoker threads

    sleep(1);

    printf("smoker threads created\n\n");

    pthread_create(&agent_thread, NULL, agent, NULL); // Create agent thread

    sleep(1);

    for (int i = 0; i < N; i++)
        pthread_join(f_threads[i], NULL); // Wait for f threads to finish
    printf("f threads joined\n");

    for (int i = 0; i < N; i++)
        pthread_join(smoker_threads[i], NULL); // Wait for smoker threads to finish
    printf("smoker threads joined\n");

    pthread_join(agent_thread, NULL); // Wait for agent thread to finish
    printf("agent thread joined\n");

    return 0; // Wait for threads to finish (not reachable in this case)
}