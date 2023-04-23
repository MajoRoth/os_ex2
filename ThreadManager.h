//
// Created by amitroth on 4/23/23.
//

#ifndef OS_EX2_THREADMANAGER_H
#define OS_EX2_THREADMANAGER_H

#include <map>
#include <memory>
#include <vector>
#include <queue>
#include "Thread.h"

typedef std::shared_ptr<Thread> ThreadPointer;

class ThreadManager{

private:
    std::map<int, ThreadPointer> threads;
    int size;  /* Is it necessery? */

    std::priority_queue <int, std::vector<int>, std::greater<int> > minHeap;



public:
    int get_first_available_id();

    ThreadManager(void);

    int append_thread(ThreadPointer thread_ptr);
    int remove_thread(ThreadPointer thread_ptr);

    /* CHECK WHY IT IS NOT WORKING */
    friend std::ostream& operator<< (std::ostream& stream, const ThreadManager & thread_manager);

    void debug();
};

#endif //OS_EX2_THREADMANAGER_H
