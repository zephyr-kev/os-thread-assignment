// ----------------------------
// | COMPILE USING -std=c++11 |
// ----------------------------

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

const int ARRAY_SIZE = 20;
const int MAX_NUM = 200;

int num_array[ARRAY_SIZE];

int even_sum = 0;
int odd_sum = 0;
int total_sum = 0;

mutex mtx;
condition_variable even_cv;
condition_variable odd_cv;

bool evenFinished = false;
bool oddFinished = false;

// Fill array with random numbers. Max number is MAX_NUM.
void populateRandom() {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        num_array[i] = rand() % MAX_NUM + 1;
    }
}

// Fill array with sequential values based on ARRAY_SIZE.
void populateFixed() {
    for (int i = 0; i < ARRAY_SIZE; i++) {
            num_array[i] = i + 1;
    }
}

void returnSum() {
    cout << "Even sum: " << even_sum << endl;
    cout << "Odd sum: " << odd_sum << endl;
    cout << "Total sum: " << total_sum << endl;
}

// Sum even indicies.
void thread_even() {
    int sum = 0;
    for (int i = 0; i < ARRAY_SIZE; i += 2) {
        sum += num_array[i];
    }

    // This thread has the lock to update shared variable.
    // To my understanding, it will get unlocked after the thread is finished.
    lock_guard<mutex> lock(mtx);
    even_sum = sum;
    evenFinished = true;

    even_cv.notify_one(); // Notify thread d that thread b is complete.
}

void thread_odd() {
    // Sum odd indicies.
    int sum = 0;
    for (int i = 1; i < ARRAY_SIZE; i += 2) {
        sum += num_array[i];
    }

    // This thread has the lock to update shared variable.
    // To my understanding, it will get unlocked after the thread is finished.
    lock_guard<mutex> lock(mtx);
    odd_sum = sum;
    oddFinished = true;

    odd_cv.notify_one(); // Notify thread d that thread c is complete.
}

void thread_sum() {
    unique_lock<mutex> lock(mtx); // This thread has the lock to access odd_sum and even_sum (really all global variables.)
    even_cv.wait(lock, [] { return evenFinished; }); // Wait until thread handling even is finished.
    odd_cv.wait(lock, [] { return oddFinished; }); // Wait until thread handing odd is finished.
    total_sum = odd_sum + even_sum;
}

int main() {
    cout << endl;
    srand(time(0)); // Seed random number generator.

    // Just for testing, I am running doing this with fixed and random values.
    
    cout << "Fixed numbers." << endl;
    populateFixed();

    thread b(thread_even);
    thread c(thread_odd);
    thread d(thread_sum);

    b.join();
    c.join();
    d.join();

    returnSum();
    cout << endl;

    cout << "Random numbers." << endl;
    populateRandom();

    thread e(thread_even);
    thread f(thread_odd);
    thread g(thread_sum);

    e.join();
    f.join();
    g.join();

    returnSum();
    cout << endl;

    return 0;
}