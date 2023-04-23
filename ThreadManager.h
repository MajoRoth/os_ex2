//
// Created by amitroth on 4/23/23.
//

#ifndef OS_EX2_THREADMANAGER_H
#define OS_EX2_THREADMANAGER_H

#include <map>
#include <memory>

#include "Thread.h"

typedef std::shared_ptr<Thread> ThreadPointer;

class ThreadManager{

private:
    std::map<int, ThreadPointer> threads;
    int size;  /* Is it necessery? */

    bool check_if_id_in_threads(int id);



public:
    int get_max_id();
    int get_first_available_id();

    int append_thread(ThreadPointer thread_ptr);
    int remove_thread(ThreadPointer thread_ptr);

    /* CHECK WHY IT IS NOT WORKING */
    friend std::ostream& operator<< (std::ostream& stream, const ThreadManager & thread_manager);

    void debug();
};


#endif //OS_EX2_THREADMANAGER_H
