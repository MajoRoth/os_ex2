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
    std::priority_queue <int, std::vector<int>, std::greater<int> > minHeap;

public:
    ThreadManager(void);

    int get_first_available_id();
    int getSize() const {return threads.size();};

    int append_thread(void (*f)(void));
    int delete_thread(int id);

    bool exists(int tid) const {return threads.find(tid) != threads.end();};
    bool has_available_space() const {return getSize() <= MAX_THREAD_NUM;};

    friend std::ostream& operator<< (std::ostream& stream, const ThreadManager & thread_manager);
};

#endif //OS_EX2_THREADMANAGER_H
