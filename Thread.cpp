//
// Created by Amit Roth on 21/04/2023.
//

#include "Thread.h"

Thread::Thread(int id, void (*f)(void)) {
    this->id = id;
}
