/*
 * User-Level Threads Library (uthreads)
 * Hebrew University OS course.
 * Author: OS, os@cs.huji.ac.il
 */
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <deque>
#include <queue>
#include <set>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>
#include <algorithm>


#include "uthreads.h"
#include "Thread.h"
#define RED "\x1B[31m"
#define RESET "\x1B[0m"

enum ERR{SYS_ERR, UTHREADS_ERR};

int scheduler(void);

typedef std::shared_ptr<Thread> ThreadPointer;
std::map<int, ThreadPointer> threads;
std::priority_queue <int, std::vector<int>, std::greater<int> > minHeap;

//Scheduler
std::deque<int> ready_queue;
std::set<int> blocked_set;
std::set<int> sleeping_set;
int running_id;
State running_thread_state;

// Timer
struct itimerval timer;
int totalQuantum;

// Signals
struct sigaction sa;
sigset_t signals_set;

/**
 * PRIVATE FUNCTIONS DECLERATIONS
 */

// system functions
int error(ERR err, const std::string& text);
void BLOCK_SIGNALS();
void UNBLOCK_SIGNALS();
void debug();

// threads manager functions
int get_first_available_id();
int append_thread(void (*f)());
int delete_thread(int id);





int error(ERR err, const std::string& text){
    if (err == SYS_ERR){
        std::cerr << "system error: " << text <<std::endl;
        exit(1);
    }
    else{
        std::cerr << "thread library error: " << text <<std::endl;
        return -1;
    }
}
void BLOCK_SIGNALS(){
    if (sigprocmask(SIG_BLOCK, &signals_set, nullptr)){
        error(SYS_ERR, "sigprocmask failed");
    }
}

void UNBLOCK_SIGNALS(){
    if (sigprocmask(SIG_UNBLOCK, &signals_set, nullptr)){
        error(SYS_ERR, "sigprocmask failed");
    }
}

int get_first_available_id() {
    int firstID = minHeap.top();
    minHeap.pop();
    return firstID;
}

int append_thread(void (*f)()){
    ThreadPointer thread_ptr (new Thread(get_first_available_id(), f));
    threads[thread_ptr->getId()] = thread_ptr;
    if (thread_ptr->getId() != 0)
    {
        ready_queue.push_back(thread_ptr->getId());
    }
    return thread_ptr->getId();
}

void debug () {
    std::cout << " --- Printing uthreads library data --- " << std::endl;
    std::cout << "running_id: " << running_id << " running_thread_state: " << running_thread_state << " totalQuantum: " << totalQuantum << std::endl;

    std::cout << "Ready Queue: " << std::endl;
    for(const auto& id: ready_queue){
        std::cout << id << " ";
    }
    std::cout << "Blocked Set: " << std::endl;
    for(const auto& id: blocked_set){
        std::cout << id << " ";
    }
    std::cout << std::endl;
    std::cout << "Sleeping Set: " << std::endl;
    for(const auto& id: sleeping_set){
        std::cout << id << " ";
    }
    std::cout << std::endl;

    std::cout << "Blocked Set: " << std::endl;
    for(const auto& id: blocked_set){
        std::cout << id << " ";
    }
    std::cout << std::endl;

    std::cout << "Sleeping Set: " << std::endl;
    for(const auto& id: sleeping_set){
        std::cout << id << " ";
    }
    std::cout << std::endl;

    std::cout << "Threads: " << std::endl;

    for(const auto& thread: threads){
        std::cout << *thread.second;
    }
    std::cout << " ------------------------------------- " << std::endl;
}

int delete_thread(int id) {
    minHeap.push(id);
    threads.erase(id);
}

int get_threads_size(){
    return threads.size();
}

bool does_thread_exist(int tid){
    return threads.find(tid) != threads.end();
}

bool has_available_space(){
    return get_threads_size() < MAX_THREAD_NUM;
}

void timer_handler(int sig)
{
    BLOCK_SIGNALS();
    totalQuantum++;
    scheduler();
}

int scheduler(){
    BLOCK_SIGNALS();
    if (running_id != -1) {
         int returned_val = sigsetjmp(threads.at(running_id)->getEnvironmentData(), 1);
        if (returned_val != 0)
        {
            UNBLOCK_SIGNALS();
            return 0;
        }
        if (running_thread_state == SLEEP) {
            sleeping_set.insert(running_id);
        } else if (running_thread_state == BLOCKED) {
            blocked_set.insert(running_id);
        } else if (running_thread_state == TERMINATE) {
            delete_thread(running_id);
        } else {
            ready_queue.push_back(running_id);
        }
    }

    // update sleeping threads
    auto id = sleeping_set.begin();
    while (id != sleeping_set.end()){
        if (threads[*id]->decrement_quantums_to_sleep() == 0){
            if (blocked_set.find(*id) == blocked_set.end()){
                ready_queue.push_back(*id);
            }
            id = sleeping_set.erase(id);
        }
        id++;
    }

    if (!ready_queue.empty()){
        running_id = ready_queue.front();
        ready_queue.pop_front();
        running_thread_state = RUNNING;
    }
    else{
        running_id = 0;
    }

    threads[running_id]->incQuantum();

    //debug();
    auto nextThreadEnv = threads.at(running_id)->getEnvironmentData();
    UNBLOCK_SIGNALS();
    if(setitimer(ITIMER_VIRTUAL, &timer, NULL)){
        error(SYS_ERR, "setitimer failed");
    }

    siglongjmp(nextThreadEnv, 1);
}

void signals_init(int quantum_usecs){
    timer.it_value.tv_sec = quantum_usecs / 1000000;
    timer.it_value.tv_usec = quantum_usecs % 1000000;
    timer.it_interval.tv_sec = quantum_usecs / 1000000;
    timer.it_interval.tv_usec = quantum_usecs % 1000000;

    if(sigemptyset(&signals_set)){
        error(SYS_ERR, "sigemptyset failed");
    }

    if(sigemptyset(&sa.sa_mask)){
        error(SYS_ERR, "sigemptyset failed");
    }

    if(sigaddset(&signals_set, SIGVTALRM)){
        error(SYS_ERR, "sigaddset failed");
    }

    sa.sa_handler = &timer_handler;

    if(sigaction(SIGVTALRM, &sa, NULL)){
        error(SYS_ERR, "sigaction failed");
    }

    if(setitimer(ITIMER_VIRTUAL, &timer, NULL)){
        error(SYS_ERR, "setitimer failed");
    }
}

std::deque<int>::iterator iter_in_ready_queue(int id){
    return std::find(ready_queue.begin(), ready_queue.end(), id);
}


/**
 * PUBLIC FUNCTIONS
 */

int uthread_init(int quantum_usecs){
    //std::cout << "UTHREAD INIT" << std::endl;
    if (quantum_usecs <= 0){
        return error(UTHREADS_ERR, "quantum_usecs must be positive");
    }

    for (int i = 0; i < MAX_THREAD_NUM; i++)
    {
        minHeap.push(i);
    }
    totalQuantum = 1;

    signals_init(quantum_usecs);

    append_thread(nullptr);
    running_id = 0;
    scheduler();
    return 0;
}

int uthread_spawn(thread_entry_point entry_point){
    BLOCK_SIGNALS();
    if (entry_point == nullptr){
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "uthread_spawn() cannot receive nullptr as an input");
    }
    if(has_available_space()){
        int id = append_thread(entry_point);
        UNBLOCK_SIGNALS();
        return id;
    }
    UNBLOCK_SIGNALS();
    return error(UTHREADS_ERR, "concurrent threads exceeded limit");
}

int uthread_terminate(int tid){
    BLOCK_SIGNALS();
    //printf(RED "stop" RESET);
    if (tid == 0){
        exit(0);
    }
    if (!does_thread_exist(tid))
    {
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "tid " + std::to_string(tid) + " doesn't exist in threads list");
    }
    auto terminatedThreadIterator = iter_in_ready_queue(tid);
    if (terminatedThreadIterator != ready_queue.end()){
        ready_queue.erase(terminatedThreadIterator);
    }
    if(blocked_set.find(tid) != blocked_set.end() )
    {
        blocked_set.erase(tid);
    }
    if(sleeping_set.find(tid) != sleeping_set.end()){
        sleeping_set.erase(tid);
    }
    if (tid == running_id) {
        running_thread_state = TERMINATE;
        UNBLOCK_SIGNALS();
        timer_handler(0);
        return 0;
    }
    delete_thread(tid);
    UNBLOCK_SIGNALS();
    return 0;
}

int uthread_block(int tid){
    BLOCK_SIGNALS();
    if (tid == 0){
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "tid 0 cannot be blocked");
    }
    if (!does_thread_exist(tid))
    {
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "tid " + std::to_string(tid) + " doesn't exist in threads list");
    }

    auto terminatedThreadIterator = iter_in_ready_queue(tid);
    if (terminatedThreadIterator != ready_queue.end()){
        ready_queue.erase(terminatedThreadIterator);
        blocked_set.insert(tid);
    }
    if(blocked_set.find(tid) != blocked_set.end() )
    {
        UNBLOCK_SIGNALS();
        return 0;
    }
    if(sleeping_set.find(tid) != sleeping_set.end()){
        sleeping_set.erase(tid);
        blocked_set.insert(tid);
    }
    if (tid == running_id) {
        running_thread_state = BLOCKED;
        UNBLOCK_SIGNALS();
        timer_handler(0);
        return 0;
    }
    UNBLOCK_SIGNALS();
}

int uthread_resume(int tid){
    BLOCK_SIGNALS();
    if (!does_thread_exist(tid))
    {
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "tid " + std::to_string(tid) + " doesn't exist in threads list");
    }

    if(blocked_set.find(tid) != blocked_set.end() ){
        if (sleeping_set.find(tid) == sleeping_set.end()){
            ready_queue.push_back(tid);
        }
        blocked_set.erase(tid);
    }
    sigemptyset(&signals_set);
    sigaddset(&signals_set, SIGVTALRM);
    UNBLOCK_SIGNALS();
    return 0;
}

int uthread_sleep(int num_quantums){
    BLOCK_SIGNALS();
    if (running_id == 0)
    {
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "main thread cannot sleep");
    }
    if (!does_thread_exist(running_id))
    {
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "tid " + std::to_string(running_id) + " doesn't exist in threads list");
    }
    running_thread_state = SLEEP;
    threads[running_id]->set_quantums_to_sleep(num_quantums);
    timer_handler(0);
    UNBLOCK_SIGNALS();
    return 0;
}

int uthread_get_tid(){
    return running_id;
}

int uthread_get_total_quantums(){
    return totalQuantum;
}

int uthread_get_quantums(int tid){
    BLOCK_SIGNALS();
    if (!does_thread_exist(tid))
    {
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "tid " + std::to_string(tid) + " doesn't exist in threads list");
    }
    int q = threads[tid]->getQuantum();
    UNBLOCK_SIGNALS();
    return q;
}


