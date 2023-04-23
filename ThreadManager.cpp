//
// Created by amitroth on 4/23/23.
//

#include "ThreadManager.h"
#include <iostream>


int ThreadManager::append_thread(ThreadPointer thread_ptr){
    std::cout << thread_ptr->getId() << std::endl;
    this->threads[thread_ptr->getId()] = thread_ptr;

    return 0;
}

int ThreadManager::get_max_id(){
    return this->threads.rbegin()->first;
}

bool ThreadManager::check_if_id_in_threads(int id) {
//    for (it = threads.begin(); it != threads.end(); it++)
//    {
//        std::cout << it->first    // string (key)
//                  << ':'
//                  << it->second   // string's value
//                  << std::endl;
//    }

    return true;
}

int ThreadManager::get_first_available_id() {
    int current_available_id = 0;


    return 0;
}

std::map<int, ThreadPointer>::iterator ThreadManager::get_first_thread() {
    return this->threads.begin();
}

std::map<int, ThreadPointer>::iterator ThreadManager::get_last_thread() {
    return this->threads.end();
}

std::ostream &operator<<(std::ostream &stream, ThreadManager &thread_manager) {
    std::map<int, ThreadPointer>::iterator it;
    stream << "Printing Thread Manager's data" << std::endl;
    stream << "Threads: " << std::endl;
    for (it = thread_manager.get_first_thread(); it != thread_manager.get_last_thread(); it++)
    {
        stream << it->first    // string (key)
                  << ':'
                  << it->second   // string's value
                  << std::endl;
    }
}


