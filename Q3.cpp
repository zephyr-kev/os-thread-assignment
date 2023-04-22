// COMPILE USING -std=c++11

#include <iostream>
#include <thread>
#include <semaphore.h>
#include <queue>
#include <mutex>
#include <ctime>
#include <condition_variable>

using namespace std;

struct requestStructure {
    int request_id;
    string ip_address;
    string page_requested;
};
queue<requestStructure> msg_queue;

string webpages[10] = {
    "https://www.google.com", "https://www.youtube.com", 
    "https://duckduckgo.com", "https://www.github.com",
    "https://www.spotify.com", "https://www.microsoft.com",
    "https://www.apple.com", "https://ubuntu.com",
    "https://archlinux.org", "https://store.steampowered.com"
};

mutex mtx;
condition_variable cv;

int request_id_counter = 0;

void listen() {
    while (true) {
        int rng = rand() % 3 + 1;
        this_thread::sleep_for(chrono::seconds(rng)); // Sleep based on random number 1 to 3.

        // Create request
        requestStructure aRequest;
        request_id_counter++;
        aRequest.request_id = request_id_counter;
        aRequest.ip_address = "";
        aRequest.page_requested = webpages[rand() % 10]; // Get random webpage from array.

        // Add request to message queue
        unique_lock<mutex> lock(mtx); // This thread holds the lock.
        msg_queue.push(aRequest);
        cout << "Recieved request: Request ID: " << aRequest.request_id << " Page requested: " << aRequest.page_requested << endl;
        lock.unlock(); // Release the lock.

        cv.notify_one(); // Notify one thread to consume request.
    }
}

void do_request(int thread_id) {
    while (true) {
        unique_lock<mutex> lock(mtx); // Now this thread holds the lock
        cv.wait(lock, []{ return !msg_queue.empty(); }); // Wait until message queue is not empty.

        // Get request
        requestStructure getRequest;
        getRequest = msg_queue.front(); // Get the request at the front of the queue.
        msg_queue.pop(); // Remove the request at the top of the queue.
        lock.unlock(); // Release the lock

        // Do something with the request.
        cout << "Thread " << thread_id << " completed request: Request ID: " << getRequest.request_id << " requesting webpage: " << getRequest.page_requested << endl;
    }
}


int main() {
    srand(time(0)); // Seed random number generator.

    thread listener(listen); // Listener thread

    // Create worker threads based on num_threads.
    const int num_threads = 4;
    thread workers[num_threads];
    for (int i = 0; i < num_threads; i++) {
        workers[i] = thread(do_request, i + 1);
    }

    // Join threads
    listener.join();
    for (int i = 0; i < num_threads; i++) {
        workers[i].join();
    }

    return 0;
}
