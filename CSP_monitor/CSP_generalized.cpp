#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <random>
#include <chrono>
# define N 3

using namespace std;
    
class CigaretteSmokingProblem {

private:
    mutex mtx;
    condition_variable next_round;
    array<condition_variable, N> smokers;
    int current_smoker = -1; // Shared state to track the current smoker

public:
    void dropIngredients(array<int, N-1> items) {
        unique_lock<mutex> lock(mtx);

        next_round.wait(lock, [this] { return current_smoker == -1; }); // Wait for the next round

        cout << "Agent has dropped ingredients : [ ";

        for (int i = 0; i < N-1; ++i) {
            cout << items[i] << " ";
        }

        cout << "]" << endl;

        int missing = 0;

        for (int i = 0; i < N; ++i) missing ^= i; // XOR of all items

        for (auto i : items) {
            missing ^= i; // XOR to find the missing item
        }

        current_smoker = missing; // Set the smoker who can proceed
        smokers[missing].notify_one(); // Notify the corresponding smoker
    }

    void smoker(int id) {
        unique_lock<mutex> lock(mtx);

        while (true) {
            smokers[id].wait(lock, [this, id] { 
                return current_smoker == id; // Wait until it's this smoker's turn
            });

            cout << "Smoker having item " << id << " got rest of the ingredients and is smoking now..." << endl;

            this_thread::sleep_for(chrono::seconds(1)); // Simulate smoking time

            cout << "Smoking finished !!" << endl;
            cout << endl;

            current_smoker = -1; // Reset the state for the next round
            next_round.notify_one(); // Notify the agent for the next round
        }
    }
};

CigaretteSmokingProblem csp;

void agent() {
    while (true) {
        int i = rand() % N; // Randomly select one ingredient to exclude

        array<int, N-1> items = {0}; // Properly initialize the array

        int index = 0;
        for (int j = 0; j < N; ++j) {
            if (j != i) {
                items[index++] = j; // Fill the array with the ingredients excluding the selected one
            }
        }

        // random_device rd;
        // mt19937 g(rd());
        // shuffle(items.begin(), items.end(), g);        

        csp.dropIngredients(items); // Ready to drop the ingredients
    }
}

int main () {

    srand(time(0)); // Seed for random number generation

    array<thread, N> smoker_threads;

    for (int i = 0; i < N; ++i) {
        smoker_threads[i] = thread(&CigaretteSmokingProblem::smoker, &csp, i);
    }

    thread agent_thread(agent);

    // join threads

    for (auto& t : smoker_threads) {
        t.join();
    }
    agent_thread.join();

    return 0;
}