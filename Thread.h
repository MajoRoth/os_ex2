//
// Created by Amit Roth on 21/04/2023.
//

#ifndef EX2_THREAD_H
#define EX2_THREAD_H

#include <setjmp.h>
#include "uthreads.h"
#include <iostream>

enum State{
    RUNNING, BLOCKED, READY
};

class Thread{

private:
    int id;
    int quantum;
    sigjmp_buf environmentData;
    State state;
    char stack[STACK_SIZE];

public:
    /* Constructors */
    Thread(int id, void (*f)(void));

    /* Setters */
    //void setQuantom(int quantom_usecs): quantom(quantom_usecs);
    void setState(State newState) {state=newState;}
    void incQuantum(void) {quantum++;}

    /* Getters */
    int getId() const {return id;}
    int getQuantum() const {return quantum;}
    State getState() const {return state;}
    sigjmp_buf &getEnvironmentData() {return environmentData;}

    friend std::ostream& operator<< (std::ostream& stream, const Thread & thread);













};




#endif //EX2_THREAD_H



