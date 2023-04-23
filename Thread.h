//
// Created by Amit Roth on 21/04/2023.
//

#ifndef EX2_THREAD_H
#define EX2_THREAD_H

#include <setjmp.h>
#include "uthreads.h"

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
    int getId() {return id;}
    int getQuantum() {return quantum;}
    State getState() {return state;}
    sigjmp_buf &getEnvironmentData() {return environmentData;}










};




#endif //EX2_THREAD_H



