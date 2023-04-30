//
// Created by Amit Roth on 21/04/2023.
//

#ifndef EX2_THREAD_H
#define EX2_THREAD_H
#include <setjmp.h>
#include "uthreads.h"
#include <iostream>

enum State{
    RUNNING, BLOCKED, READY, TERMINATE, SLEEP
};

class Thread{
private:
    int id;
    int quantum;
    sigjmp_buf environmentData;
    char stack[STACK_SIZE];
    int sleepQuantumsLeft;

public:
    /* Constructors */
    Thread(int id, void (*f)(void));

    /* Setters */
    void incQuantum(void){quantum++;}
    void set_quantums_to_sleep(int quantums){sleepQuantumsLeft = quantums;};

    /* Getters */
    int getId() const {return id;}
    int getQuantum() const {return quantum;}
    sigjmp_buf &getEnvironmentData() {return environmentData;}

    int decrement_quantums_to_sleep();

    friend std::ostream& operator<< (std::ostream& stream, const Thread & thread);
};

#endif //EX2_THREAD_H
