#include <iostream>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <array>
#include <random>
#include <chrono>
#include <atomic>
#include <unistd.h> 

#define N 10

using namespace std;

class CigaretteSmokingProblem {

private:

    mutex mtx;
    condition_variable next_round;
    array<condition_variable, N> smokers;
    int current_smoker = -1;

public:

    void dropIngredients(array<int, N - 1> ingredients) {
        unique_lock<mutex> lock(mtx);

        next_round.wait(lock, [this] { return current_smoker == -1; });

        printf("Agent: Producing items: ");
        for(auto i : ingredients) printf("%d ", i);
        printf("\n");

        int missing = 0; 
        for (int i = 0; i < N; ++i) missing ^= i; // XOR all ingredients
        for (auto i : ingredients) missing ^= i; // Remove the produced items

        current_smoker = missing;   // found the missing ingridient
        smokers[missing].notify_one();

    }

    void pickupIngredients(int id) {
        unique_lock<mutex> lock(mtx);

        smokers[id].wait(lock, [this, id] { return current_smoker == id; });
    }

    void notifyForNextRound() {
        unique_lock<mutex> lock(mtx);

        current_smoker = -1;
        next_round.notify_one();
    }
};

CigaretteSmokingProblem csp;

void* smoker(void* arg) {
    int id = *(int*)arg;

    while (true) {

        csp.pickupIngredients(id);

        printf("Smoker %d: Smoking...\n", id);
        sleep(1); // simulation of smoking time
        printf("Smoker %d: Finished smoking\n\n", id);

        csp.notifyForNextRound();
    }

    return nullptr;
}

void* agent(void* arg) {

    while (true) {

        int exclude = rand() % N; // Randomly select an item to exclude

        array<int, N - 1> items;

        int index = 0;

        for (int j = 0; j < N; ++j) {
            if (j != exclude) {
                items[index++] = j;
            }
        }

        // Send the items to the monitor to drop on the table

        csp.dropIngredients(items);
    
    }

    return nullptr;
}

int main() {
    srand(time(0));

    // Initializing threads for smokers and agent
    pthread_t smoker_threads[N];
    pthread_t agent_thread;

    // Creating threads for smokers
    int smoker_ids[N];
    for (int i = 0; i < N; ++i) {
        smoker_ids[i] = i;
        pthread_create(&smoker_threads[i], nullptr, smoker, &smoker_ids[i]);
    }
    printf("smoker threads created\n");

    sleep(1); 

    // Creating thread for agent
    pthread_create(&agent_thread, nullptr, agent, nullptr);
    printf("agent thread created\n\n");

    sleep(1);

    // Joining threads
    for (int i = 0; i < N; ++i) {
        pthread_join(smoker_threads[i], nullptr);
    }
    printf("smoker threads joined\n");

    // Joining agent thread
    pthread_join(agent_thread, nullptr);   
    printf("agent thread joined\n");

    return 0;
}