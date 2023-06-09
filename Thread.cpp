//
// Created by Amit Roth on 21/04/2023.
//

#include "Thread.h"
#include <setjmp.h>
#include <signal.h>
#include <iostream>

#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
        "rol    $0x11,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#else
/* code for 32-bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5


/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
                 "rol    $0x9,%0\n"
            : "=g" (ret)
            : "0" (addr));
    return ret;
}


#endif

Thread::Thread(int id, void (*f)(void)) : id(id), quantum(0)
{
    address_t sp = (address_t) stack + STACK_SIZE - sizeof(address_t);
    address_t pc = (address_t) f;
    sigsetjmp(environmentData, 1);
    (environmentData->__jmpbuf)[JB_SP] = translate_address(sp);
    (environmentData->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&environmentData->__saved_mask);
}


std::ostream& operator<< (std::ostream& stream, const Thread & thread){
    stream << "Thread {id=" << thread.getId() << "}: quantum=" << thread.getQuantum() << std::endl;
    return stream;
}

int Thread::decrement_quantums_to_sleep() {
    sleepQuantumsLeft--;
    if (sleepQuantumsLeft == 0){
        return 0;
    }
    return 1;
}

