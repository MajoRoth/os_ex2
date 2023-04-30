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


// Threads Manager
typedef std::shared_ptr<Thread> ThreadPointer;
int runningId;
State runningThreadState;

// Data Structures
std::map<int, ThreadPointer> threads;
std::deque<int> readyQueue;
std::set<int> blockedSet;
std::set<int> sleepingSet;
std::priority_queue <int, std::vector<int>, std::greater<int>> minHeap;

// Timer
struct itimerval timer;
int totalQuantum;

// Signals
struct sigaction sa;
sigset_t signalsSet;

enum ERR{SYS_ERR, UTHREADS_ERR};

/**
 * PRIVATE FUNCTIONS DECLERATIONS
 */

// system functions
int error(ERR err, const std::string& text);
void BLOCK_SIGNALS();
void UNBLOCK_SIGNALS();
void timer_handler(int sig);
void signals_init(int quantumUsecs);
void debug();

// threads manager functions
int get_first_available_id();
int append_thread(void (*f)());
void delete_thread(int id);
int get_threads_size();
bool does_thread_exist(int tid);
bool has_available_space();
std::deque<int>::iterator iter_in_ready_queue(int id);
int scheduler();


/**
 * PUBLIC FUNCTIONS
 */

int uthread_init(int quantum_usecs){
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
    runningId = 0;
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
    if (tid == 0){
        exit(0);
    }
    if (!does_thread_exist(tid))
    {
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "tid " + std::to_string(tid) + " doesn't exist in threads list");
    }
    auto terminatedThreadIterator = iter_in_ready_queue(tid);
    if (terminatedThreadIterator != readyQueue.end()){
        readyQueue.erase(terminatedThreadIterator);
    }
    if(blockedSet.find(tid) != blockedSet.end() )
    {
        blockedSet.erase(tid);
    }
    if(sleepingSet.find(tid) != sleepingSet.end()){
        sleepingSet.erase(tid);
    }
    if (tid == runningId) {
        runningThreadState = TERMINATE;
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
    if (terminatedThreadIterator != readyQueue.end()){
        readyQueue.erase(terminatedThreadIterator);
        blockedSet.insert(tid);
    }
    if(blockedSet.find(tid) != blockedSet.end() )
    {
        UNBLOCK_SIGNALS();
        return 0;
    }
    if(sleepingSet.find(tid) != sleepingSet.end()){
        blockedSet.insert(tid);
    }
    if (tid == runningId) {
        runningThreadState = BLOCKED;
        UNBLOCK_SIGNALS();
        timer_handler(0);
        return 0;
    }
    UNBLOCK_SIGNALS();
    return 0;
}

int uthread_resume(int tid){
    BLOCK_SIGNALS();
    if (!does_thread_exist(tid))
    {
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "tid " + std::to_string(tid) + " doesn't exist in threads list");
    }

    if(blockedSet.find(tid) != blockedSet.end() ){
        if (sleepingSet.find(tid) == sleepingSet.end()){
            readyQueue.push_back(tid);
        }
        blockedSet.erase(tid);
    }
    sigemptyset(&signalsSet);
    sigaddset(&signalsSet, SIGVTALRM);
    UNBLOCK_SIGNALS();
    return 0;
}

int uthread_sleep(int num_quantums){
    BLOCK_SIGNALS();
    if (runningId == 0)
    {
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "main thread cannot sleep");
    }
    if (!does_thread_exist(runningId))
    {
        UNBLOCK_SIGNALS();
        return error(UTHREADS_ERR, "tid " + std::to_string(runningId) + " doesn't exist in threads list");
    }
    runningThreadState = SLEEP;
    threads[runningId]->set_quantums_to_sleep(num_quantums);
    timer_handler(0);
    UNBLOCK_SIGNALS();
    return 0;
}

int uthread_get_tid(){
    return runningId;
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

/**
 * Private Functions Implementation
 */

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
    if (sigprocmask(SIG_BLOCK, &signalsSet, nullptr)){
        error(SYS_ERR, "sigprocmask failed");
    }
}

void UNBLOCK_SIGNALS(){
    if (sigprocmask(SIG_UNBLOCK, &signalsSet, nullptr)){
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
        readyQueue.push_back(thread_ptr->getId());
    }
    return thread_ptr->getId();
}

void debug () {
    std::cout << " --- Printing uthreads library data --- " << std::endl;
    std::cout << "runningId: " << runningId << " runningThreadState: " << runningThreadState << " totalQuantum: " << totalQuantum << std::endl;

    std::cout << "Ready Queue: " << std::endl;
    for(const auto& id: readyQueue){
        std::cout << id << " ";
    }
    std::cout << "Blocked Set: " << std::endl;
    for(const auto& id: blockedSet){
        std::cout << id << " ";
    }
    std::cout << std::endl;
    std::cout << "Sleeping Set: " << std::endl;
    for(const auto& id: sleepingSet){
        std::cout << id << " ";
    }
    std::cout << std::endl;

    std::cout << "Blocked Set: " << std::endl;
    for(const auto& id: blockedSet){
        std::cout << id << " ";
    }
    std::cout << std::endl;

    std::cout << "Sleeping Set: " << std::endl;
    for(const auto& id: sleepingSet){
        std::cout << id << " ";
    }
    std::cout << std::endl;

    std::cout << "Threads: " << std::endl;

    for(const auto& thread: threads){
        std::cout << *thread.second;
    }
    std::cout << " ------------------------------------- " << std::endl;
}

void delete_thread(int id) {
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
    if (runningId != -1) {
        int returned_val = sigsetjmp(threads.at(runningId)->getEnvironmentData(), 1);
        if (returned_val != 0)
        {
            UNBLOCK_SIGNALS();
            return 0;
        }
        if (runningThreadState == SLEEP) {
            sleepingSet.insert(runningId);
        } else if (runningThreadState == BLOCKED) {
            blockedSet.insert(runningId);
        } else if (runningThreadState == TERMINATE) {
            delete_thread(runningId);
        } else {
            readyQueue.push_back(runningId);
        }
    }

    // update sleeping threads
    auto id = sleepingSet.begin();
    while (id != sleepingSet.end()){
        if (threads[*id]->decrement_quantums_to_sleep() == 0){
            if (blockedSet.find(*id) == blockedSet.end()){
                readyQueue.push_back(*id);
            }
            id = sleepingSet.erase(id);
        }
        else{
            id++;
        }
    }

    if (!readyQueue.empty()){
        runningId = readyQueue.front();
        readyQueue.pop_front();
        runningThreadState = RUNNING;
    }
    else{
        runningId = 0;
    }

    threads[runningId]->incQuantum();

    auto nextThreadEnv = threads.at(runningId)->getEnvironmentData();
    UNBLOCK_SIGNALS();
    if(setitimer(ITIMER_VIRTUAL, &timer, NULL)){
        error(SYS_ERR, "setitimer failed");
    }

    siglongjmp(nextThreadEnv, 1);
}

void signals_init(int quantumUsecs){
    timer.it_value.tv_sec = quantumUsecs / 1000000;
    timer.it_value.tv_usec = quantumUsecs % 1000000;
    timer.it_interval.tv_sec = quantumUsecs / 1000000;
    timer.it_interval.tv_usec = quantumUsecs % 1000000;

    if(sigemptyset(&signalsSet)){
        error(SYS_ERR, "sigemptyset failed");
    }

    if(sigemptyset(&sa.sa_mask)){
        error(SYS_ERR, "sigemptyset failed");
    }

    if(sigaddset(&signalsSet, SIGVTALRM)){
        error(SYS_ERR, "sigaddset failed");
    }

    sa.sa_handler = &timer_handler;

    if(sigaction(SIGVTALRM, &sa, nullptr)){
        error(SYS_ERR, "sigaction failed");
    }

    if(setitimer(ITIMER_VIRTUAL, &timer, nullptr)){
        error(SYS_ERR, "setitimer failed");
    }
}

std::deque<int>::iterator iter_in_ready_queue(int id){
    return std::find(readyQueue.begin(), readyQueue.end(), id);
}

