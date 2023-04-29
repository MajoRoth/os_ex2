//
// Created by amitroth on 4/23/23.
//

#ifndef OS_EX2_THREADMANAGER_H
#define OS_EX2_THREADMANAGER_H

#include <map>
#include <memory>
#include <vector>
#include <queue>
#include <sys/time.h>
#include <csignal>
#include <signal.h>
#include <setjmp.h>
#include "Thread.h"

typedef std::shared_ptr<Thread> ThreadPointer;

class ThreadManager{

private:
    std::map<int, ThreadPointer> threads;
    std::priority_queue <int, std::vector<int>, std::greater<int> > minHeap;

    int quantum;
    struct itimerval timer;
    struct sigaction sa;

    int totalQuantum;


public:
    ThreadManager(int quantum);

    int get_first_available_id();
    int getSize() const {return threads.size();};

    int append_thread(void (*f)(void));
    int delete_thread(int id);

    bool exists(int tid) const {return threads.find(tid) != threads.end();};
    bool has_available_space() const {return getSize() <= MAX_THREAD_NUM;};

    void timer_handler(int sig);

    friend std::ostream& operator<< (std::ostream& stream, const ThreadManager & thread_manager);
};

#endif //OS_EX2_THREADMANAGER_H
