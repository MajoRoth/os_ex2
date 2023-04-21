/*
 * User-Level Threads Library (uthreads)
 * Hebrew University OS course.
 * Author: OS, os@cs.huji.ac.il
 */

#include "uthreads.h"



int uthread_init(int quantum_usecs){
    if (quantum_usecs <= 0){
        return -1;
    }


    return 1;
}

int uthread_spawn(thread_entry_point entry_point);

int uthread_terminate(int tid);

int uthread_block(int tid);

int uthread_resume(int tid);

int uthread_sleep(int num_quantums);

int uthread_get_tid();

int uthread_get_total_quantums();

int uthread_get_quantums(int tid);


