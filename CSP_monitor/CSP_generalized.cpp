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
    int current_smoker = -1; 

public:

    void dropIngredients(array<int, N-1> items) {
        unique_lock<mutex> lock(mtx);

        next_round.wait(lock, [this] { return current_smoker == -1; });

        cout << "Agent has dropped ingredients : [ ";
        for (int i = 0; i < N-1; ++i) {
            cout << items[i] << " ";
        }
        cout << "]" << endl;

        int missing = 0;
        for (int i = 0; i < N; ++i) missing ^= i;
        for (auto i : items) missing ^= i;

        current_smoker = missing; 
        smokers[missing].notify_one();

    }

    void pickupIngredients(int id) {
        unique_lock<mutex> lock(mtx);

        smokers[id].wait(lock, [this, id] { return current_smoker == id; });

        cout << "Smoker " << id << " picked up ingredients..." << endl;

    }

    void notify_for_next_round() {
        unique_lock<mutex> lock(mtx);

        current_smoker = -1; 
        next_round.notify_all(); 
    }

};

CigaretteSmokingProblem csp;

void smoker(int id) {
    while (true) {

        csp.pickupIngredients(id); 

        cout << "Smoker " << id << " is smoking..." << endl;

        this_thread::sleep_for(chrono::seconds(1)); // Simulate smoking time

        cout << "Smoker " << id << " finished smoking." << endl << endl;

        csp.notify_for_next_round(); 

    }
}

void agent() {

    while (true) {

        int exclude = rand() % N;

        array<int,N-1> items;

        int index = 0;
        for (int j = 0; j < N; ++j) {
            if (j != exclude) {
                items[index++] = j;
            }
        }

        csp.dropIngredients(items); 

    }
}

int main() {

    srand(time(0)); 

    array<thread, N> smoker_threads;

    for (int i = 0; i < N; ++i) {
        smoker_threads[i] = thread(smoker, i);
    }

    thread agent_thread(agent);

    for (auto& t : smoker_threads) {
        t.join();
    }

    agent_thread.join();

    return 0;
}
