#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
using namespace std;

#include "BinarySemaphore.hpp"

BinarySemaphore tobacco, paper, match, next_round;

void* agent(void* arg) {

    while (1) {

        binary_semaphore_wait(&next_round);

        printf("Agent : Producing ");

        int num = rand() % 3; // Randomly select an item to exclude

        switch(num) {
            case 0:
                printf("Paper and Match\n");
                binary_semaphore_signal(&paper);
                binary_semaphore_signal(&match);
                break;
            case 1:
                printf("Match and Tobacco\n");
                binary_semaphore_signal(&match);
                binary_semaphore_signal(&tobacco);
                break;
            case 2:
                printf("Tobacco and Paper\n");
                binary_semaphore_signal(&tobacco); 
                binary_semaphore_signal(&paper);
                break;
        }

    }
}

void* smoker_with_tobacco(void* arg) {

    while(1) {

        binary_semaphore_wait(&paper); // waiting for paper

        if(binary_semaphore_try_wait(&match) == 0) { // if match is available then take it

            printf("Smoker having tobacco : Smoking...\n");
            sleep(1); // simulation of smoking time
            printf("Smoker having tobacco : Finished smoking ! \n\n");

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

        if(binary_semaphore_try_wait(&tobacco) == 0) { // if tobacco is available then take it
            
            printf("Smoker having paper : Smoking...\n");
            sleep(1); // simulation of smoking time
            printf("Smoker having paper : Finished smoking ! \n\n");

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

        if(binary_semaphore_try_wait(&paper) == 0) { // if paper is available then take it

            printf("Smoker having match : Smoking...\n");
            sleep(1); // simulation of smoking time
            printf("Smoker having match : Finished smoking ! \n\n");

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

    // threads for agent and smokers
    pthread_t agent_thread , smoker_threads[3];

    pthread_create(&smoker_threads[0], NULL, smoker_with_tobacco, NULL);
    pthread_create(&smoker_threads[1], NULL, smoker_with_paper, NULL);
    pthread_create(&smoker_threads[2], NULL, smoker_with_match, NULL);
    printf("Smoker threads created\n");

    sleep(1);

    pthread_create(&agent_thread, NULL, agent, NULL);
    printf("Agent thread created\n");

    sleep(1);
    pthread_join(smoker_threads[0], NULL);
    pthread_join(smoker_threads[1], NULL);
    pthread_join(smoker_threads[2], NULL);
    pthread_join(agent_thread, NULL);
    printf("All threads joined\n");

    return 0; 
}
