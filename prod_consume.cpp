#include <iostream>
#include <thread>
#include <semaphore.h>
using namespace std;

const int BUFFER_SIZE = 5;
int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
sem_t empt; // counts number of empt slots in the buffer
sem_t full; // counts number of filled slots in the buffer
sem_t mtx; // binary semaphore to protect access to the buffer

void producer() {
    int item = 0;
    while (true) {
        // produce an item
        item++;
        sem_wait(&empt); // wait for an empt slot in the buffer
        sem_wait(&mtx); // acquire mtx lock to protect the buffer
        buffer[in] = item; // add the item to the buffer
        in = (in + 1) % BUFFER_SIZE;
        sem_post(&mtx); // release mtx lock
        // signal that there is one more filled slot in the buffer
        sem_post(&full);
    }
}

void consumer() {
    int item;
    while (true) {
        sem_wait(&full); // wait for a filled slot in the buffer
        sem_wait(&mtx); // acquire mtx lock to protect the buffer
        item = buffer[out]; // remove an item from the buffer
        out = (out + 1) % BUFFER_SIZE;
        sem_post(&mtx); // release mtx lock
        // signal that there is one more empt slot in the buffer
        sem_post(&empt);
        // consume the item
        std::cout << "Consumed item: " << item << std::endl;
    }
}

int main() {
    // initialize semaphores
    sem_init(&empt, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mtx, 0, 1);
    // create producer and consumer threads
    thread prod_thread(producer);
    thread cons_thread(consumer);
    // wait for threads to finish (should never happen)
    prod_thread.join();
    cons_thread.join();
    // destroy semaphores
    sem_destroy(&empt);
    sem_destroy(&full);
    sem_destroy(&mtx);
    return 0;
}