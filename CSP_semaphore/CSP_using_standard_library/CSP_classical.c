#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

char *items[3] = {"tobacco", "paper", "match"};

sem_t tobacco, paper, match, next_round;

void* agent(void* arg) {

    printf("Agent arrived and the smoking session has started \n");

    int round = 0;

    while (1) {

        sem_wait(&next_round);

        round++;
        printf(" Round %d: Agent is producing ", round);

        int num = rand() % 3; // Randomly select an item to exclude

        if(num == 0) {
            sem_post(&paper); 
            sem_post(&match);
            printf("paper and match :\n");
        }
        else if(num == 1) {
            sem_post(&tobacco); 
            sem_post(&match);
            printf("tobacco and match :\n");
        }
        else if(num == 2) {
            sem_post(&tobacco); 
            sem_post(&paper);
            printf("tobacco and paper :\n");
        }

        // switch (num)
        // {
        //     case 0:
        //         sem_post(&paper); 
        //         sem_post(&match);
        //         printf("paper and match :\n");
        //     case 1:
        //         sem_post(&tobacco); 
        //         sem_post(&match);
        //         printf("tobacco and match :\n");
        //     case 2:
        //         sem_post(&tobacco); 
        //         sem_post(&paper);
        //         printf("tobacco and paper :\n");
    
        // }
    }
}

void* smoker_with_tobacco(void* arg) {

    while(1) {

        sem_wait(&paper); // waiting for paper

        if(sem_trywait(&match) == 0) { 

            // started smoking
            sleep(1);
            printf("Smoker having tobacco has rolled and is smoking now... \n");
            sleep(1);
            printf("Smoking finished !! \n");
            // finished smoking
            sleep(1);
            printf("\n");

            sem_post(&next_round); 

        }
        else {
            sem_post(&paper); // if match is not available, release paper
        }

    }

}

void* smoker_with_paper(void* arg) {

    while(1) {

        sem_wait(&tobacco); // waiting for tobacco

        if(sem_trywait(&match) == 0) { 
            
            // started smoking
            sleep(1);
            printf("Smoker having paper has rolled and is smoking now... \n");
            sleep(1);
            printf("Smoking finished !! \n");
            // finished smoking
            sleep(1);
            printf("\n");

            sem_post(&next_round); // notify agent for next round

        }
        else {
            sem_post(&tobacco); // if match is not available, release tobacco
        }

    }

}

void* smoker_with_match(void* arg) {

    while(1) {

        sem_wait(&tobacco); // waiting for tobacco

        if(sem_trywait(&paper) == 0) { 

            // started smoking
            sleep(1);
            printf("Smoker having match has rolled and is smoking now... \n");
            sleep(1);
            printf("Smoking finished !! \n");
            // finished smoking
            sleep(1);
            printf("\n");

            sem_post(&next_round); // notify agent for next round

        }
        else {
            sem_post(&tobacco); // if paper is not available, release tobacco
        }

    }

}

int main() {

    // semaphores for tobacco, paper and match
    sem_init(&tobacco, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&match, 0, 0);

    // semaphore for next round
    sem_init(&next_round, 0, 1);

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