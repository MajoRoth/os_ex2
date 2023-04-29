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
    int i =0;
    while(1){
        if (i %1000000000 == 0){
            i*i;
            std::cout << "f2" << std::endl;
        }
        i++;
    }
}

int main(){
    uthread_init(2000000);
    while(1){}
    uthread_spawn(f1);
    uthread_spawn(f2);
    while(1){}
}