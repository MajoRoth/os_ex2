//
// Created by Amit Roth on 21/04/2023.
//

#ifndef EX2_THREAD_H
#define EX2_THREAD_H

#include <setjmp.h>

enum State{
    RUNNING, BLOCKED, READY
};

class Thread{

private:
    int id;
    int quantom;
    sigjmp_buf environment_data;
    State state;

public:
    /* Constructors */
    Thread(int id, void (*f)(void));

    /* Setters */
    void setQuantom(int quantom_usecs): quantom(quantom_usecs);
    void setState(State state);

    /* Getters */
    int getId();
    int getQuantom();
    State getState();










};




#endif //EX2_THREAD_H



