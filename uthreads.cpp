/*
 * User-Level Threads Library (uthreads)
 * Hebrew University OS course.
 * Author: OS, os@cs.huji.ac.il
 */
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <queue>
#include <sys/time.h>
#include <csignal>
#include <signal.h>
#include <setjmp.h>


#include "uthreads.h"
#include "Thread.h"

typedef std::shared_ptr<Thread> ThreadPointer;


std::map<int, ThreadPointer> threads;
std::priority_queue <int, std::vector<int>, std::greater<int> > minHeap;

int quantum;
struct itimerval timer;
struct sigaction sa;

int totalQuantum;

/**
 * PRIVATE FUNCTIONS
 */

int get_first_available_id() {
    int firstID = minHeap.top();
    minHeap.pop();
    return firstID;
}

int append_thread(void (*f)(void)){
    ThreadPointer thread_ptr (new Thread(get_first_available_id(), f));
    threads[thread_ptr->getId()] = thread_ptr;
    return thread_ptr->getId();
}

void debug (void) {
    std::cout << "Printing Thread Manager's data" << std::endl;
    std::cout << "Threads: " << std::endl;

    for(const auto& thread: threads){
        std::cout << *thread.second;
    }
}

int delete_thread(int id) {
    minHeap.push(id);
    threads.erase(id);
}

int getSize(){
    return threads.size();
}

bool exists(int tid){
    return threads.find(tid) != threads.end();
}

bool has_available_space(){
    return getSize() <= MAX_THREAD_NUM;
}

void timer_handler(int sig)
{
    block_signals();
    totalQuantum++;
    scheduler();
}


/**
 * PUBLIC FUNCTIONS
 */

int uthread_init(int quantum_usecs){
    if (quantum_usecs <= 0){
        std::cerr << "ERROR: quantum_usecs must be positive" << std::endl;
        return -1;
    }

    for (int i = 0; i < MAX_THREAD_NUM; i++)
    {
        minHeap.push(i);
    }

    // Define The Timer
    timer.it_value.tv_sec = quantum / 100000;
    timer.it_value.tv_usec = quantum % 100000;
    timer.it_interval.tv_sec = quantum / 100000;
    timer.it_interval.tv_usec = quantum % 100000;

    totalQuantum = 1;

    sa = {0};
    sa.sa_handler = &timer_handler;

    append_thread(nullptr);
    return 0;
}

int uthread_spawn(thread_entry_point entry_point){
    if (entry_point == nullptr){
        std::cout << "ERROR: uthread_spawn() cannot recive nullptr as an input" << std::endl;
        return -1;
    }
    if(has_available_space()){
        int id = append_thread(entry_point);
        return id;
    }
    else{
        std::cout << "ERROR: concurrent threads exceeded limit" << std::endl;
        return -1;
    }


}

int uthread_terminate(int tid){
    if (tid == 0){
        //TODO: Terminate the entire program
    }
    if (!exists(tid))
    {
        return -1;
    }
    delete_thread(tid);
}

int uthread_block(int tid);

int uthread_resume(int tid);

int uthread_sleep(int num_quantums);

int uthread_get_tid();

int uthread_get_total_quantums();

int uthread_get_quantums(int tid);


