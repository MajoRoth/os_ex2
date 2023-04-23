/*
 * User-Level Threads Library (uthreads)
 * Hebrew University OS course.
 * Author: OS, os@cs.huji.ac.il
 */
#include <iostream>

#include "uthreads.h"
#include "Thread.h"
#include "ThreadManager.h"


ThreadManager threadManager;

int uthread_init(int quantum_usecs){
    if (quantum_usecs <= 0){
        std::cerr << "ERROR: quantum_usecs must be positive" << std::endl;
        return -1;
    }

    threadManager = ThreadManager();
    threadManager.append_thread(nullptr);
    return 0;
}

int uthread_spawn(thread_entry_point entry_point){
    if (entry_point == nullptr){
        std::cout << "ERROR: uthread_spawn() cannot recive nullptr as an input" << std::endl;
        return -1;
    }
    if(threadManager.has_available_space()){
        int id = threadManager.append_thread(entry_point);
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
    if (!threadManager.exists(tid))
    {
        return -1;
    }
    threadManager.delete_thread(tid);
}

int uthread_block(int tid);

int uthread_resume(int tid);

int uthread_sleep(int num_quantums);

int uthread_get_tid();

int uthread_get_total_quantums();

int uthread_get_quantums(int tid);


