#include "uthreads.h"
#include <iostream>

void f1(void){
    int i =0;
    while(1){
        if (i %1000000000 == 0){
            i*i;
            std::cout << "f1" << std::endl;
        }
        i++;
    }
}

void f2(void){
    int i =1;
    while(1){
        if (i %1000000000 == 0){
            i*i;
            std::cout << "f2" << std::endl;
        }
        i++;
    }
}

void wait_for_a_second(){
    int i =1;
    while(1){
        if (i %10000000000 == 0){
            i*i;
        }
        return;
    }
}

int main(){
    uthread_init(10000000);

    uthread_spawn(f1);
    uthread_spawn(f2);

    wait_for_a_second();
    wait_for_a_second();
    wait_for_a_second();
    while(1){}
}