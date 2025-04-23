#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <random>
#include <chrono>

using namespace std;

vector<string> items = {"tobacco", "paper", "match"};
    
class CigaretteSmokingProblem {

private:
    mutex mtx;
    condition_variable next_round;
    array<condition_variable, 3> smokers;
    int item1, item2;


public:

    CigaretteSmokingProblem() : item1(-1), item2(-1) {}

    void dropIngredients(int i1, int i2) {

        unique_lock<mutex> lock(mtx);

        next_round.wait(lock, [this] { return item1 == -1 && item2 == -1; });

        cout << "Agent is producing " <<  items[i1] << " and " << items[i2] << endl;

        item1 = i1;
        item2 = i2;
        
        int missing = 0 ^ 1 ^ 2 ^ item1 ^ item2; // XOR to find the missing item

        smokers[missing].notify_one(); // Notify the smoker who is missing the item

    }
    
    void smoker(int id) {
        unique_lock<mutex> lock(mtx);

        while (true) {
            
            smokers[id].wait(lock, [this, id] { 
                return (item1 != -1 && item2 != -1) && (item1 != id && item2 != id); 
            });

            cout << "Smoker having " << items[id] << " is smoking now..." << endl;

            this_thread::sleep_for(chrono::seconds(1)); // Simulate smoking time

            cout << "Smoking finished !!" << endl;
            cout << endl;

            item1 = -1; // Reset items after smoking
            item2 = -1;

            next_round.notify_one(); // Notify the agent for the next round
        }

    }

};

CigaretteSmokingProblem csp;

void agent() {

    array<array<int, 2>, 3> combinations = {{{0, 1}, {0, 2}, {1, 2}}}; // Possible combinations of items
    
    while(true) {

        int i = rand() % 3; // Randomly select a combination

        auto [i1, i2] = combinations[i]; // Get the selected combination

        csp.dropIngredients(i1, i2); // Drop the ingredients for the smokers

    }

}

int main () {

    srand(time(0)); // Seed for random number generation

    array<thread, 3> smoker_threads;

    for (int i = 0; i < 3; ++i) {
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