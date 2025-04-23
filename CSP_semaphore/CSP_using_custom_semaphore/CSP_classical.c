#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "BinarySemaphore.h"

char *items[3] = {"tobacco", "paper", "match"};

BinarySemaphore tobacco, paper, match, next_round;

void* agent(void* arg) {

    printf("Agent arrived and the smoking session has started \n");

    int round = 0;

    while (1) {

        binary_semaphore_wait(&next_round);

        round++;
        printf(" Round %d: Agent is producing ", round);

        int num = rand() % 3; // Randomly select an item to exclude

        if(num == 0) {
            binary_semaphore_signal(&paper);
            binary_semaphore_signal(&match);
            printf("paper and match :\n");
        }
        else if(num == 1) {
            binary_semaphore_signal(&tobacco);
            binary_semaphore_signal(&match);
            printf("tobacco and match :\n");
        }
        else if(num == 2) {
            binary_semaphore_signal(&tobacco); 
            binary_semaphore_signal(&paper);
            printf("tobacco and paper :\n");
        }

    }
}

void* smoker_with_tobacco(void* arg) {

    while(1) {

        binary_semaphore_wait(&paper); // waiting for paper

        if(binary_semaphore_try_wait(&match) == 0) { 

            // started smoking
            sleep(1);
            printf("Smoker having tobacco has rolled and is smoking now... \n");
            sleep(1);
            printf("Smoking finished !! \n");
            // finished smoking
            sleep(1);
            printf("\n");

            binary_semaphore_signal(&next_round); // notify agent for next round

        }
        else {
            
            binary_semaphore_signal(&paper); // if match is not available, release paper
        }

    }

}

void* smoker_with_paper(void* arg) {

    while(1) {

        binary_semaphore_wait(&match); // waiting for match

        if(binary_semaphore_try_wait(&tobacco) == 0) {
            
            // started smoking
            sleep(1);
            printf("Smoker having paper has rolled and is smoking now... \n");
            sleep(1);
            printf("Smoking finished !! \n");
            // finished smoking
            sleep(1);
            printf("\n");

            binary_semaphore_signal(&next_round); // notify agent for next round
        }
        else {
            binary_semaphore_signal(&match); // if tobacco is not available, release match
        }

    }

}

void* smoker_with_match(void* arg) {

    while(1) {

        binary_semaphore_wait(&tobacco); // waiting for tobacco

        if(binary_semaphore_try_wait(&paper) == 0) {

            // started smoking
            sleep(1);
            printf("Smoker having match has rolled and is smoking now... \n");
            sleep(1);
            printf("Smoking finished !! \n");
            // finished smoking
            sleep(1);
            printf("\n");

            binary_semaphore_signal(&next_round); // notify agent for next round

        }
        else {
            binary_semaphore_signal(&tobacco); // if paper is not available, release tobacco
        }

    }

}

int main() {

    // semaphores for tobacco, paper and match
    binary_semaphore_init(&tobacco, 0);
    binary_semaphore_init(&paper, 0);
    binary_semaphore_init(&match, 0);

    // semaphore for next round
    binary_semaphore_init(&next_round, 1);

    // threads for agent, f1, f2, f3 and smokers
    pthread_t agent_thread , smoker_threads[3];

    pthread_create(&smoker_threads[0], NULL, smoker_with_tobacco, NULL);
    pthread_create(&smoker_threads[1], NULL, smoker_with_paper, NULL);
    pthread_create(&smoker_threads[2], NULL, smoker_with_match, NULL);

    pthread_create(&agent_thread, NULL, agent, NULL);

    pthread_join(smoker_threads[0], NULL);
    pthread_join(smoker_threads[1], NULL);
    pthread_join(smoker_threads[2], NULL);
    pthread_join(agent_thread, NULL);

    return 0; 
}