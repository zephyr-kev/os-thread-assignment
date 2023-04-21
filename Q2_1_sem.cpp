#include <iostream>
#include <thread>
#include <semaphore.h>

using namespace std;

sem_t sem;

void runMeFirst() {
    cout << "Run me first!" << endl;
    sem_post(&sem); // Signal runMeFirst() has finished.
}

void runMeSecond() {
    sem_wait(&sem); // Wait for runMeFirst() to finish.
    cout << "I run second." << endl;
}

int main() {
    const int num_threads = 2;
    sem_init(&sem, 0, num_threads);

    thread t1(runMeFirst);
    this_thread::sleep_for(chrono::seconds(1));
    thread t2(runMeSecond);

    t1.join();
    t2.join();

    sem_destroy(&sem);

    return 0;
}