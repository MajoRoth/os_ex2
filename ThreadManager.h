//
// Created by amitroth on 4/23/23.
//

#ifndef OS_EX2_THREADMANAGER_H
#define OS_EX2_THREADMANAGER_H

typdef std::shared_ptr<Thread> ThreadPointer;

class ThreadManager{

private:
    std::set Threads;
    std::int size;  /* Is it necessery? */

public:
    int get_max_id();
    int get_first_available_id();

};

#endif //OS_EX2_THREADMANAGER_H
