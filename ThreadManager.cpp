//
// Created by amitroth on 4/23/23.
//

#include "ThreadManager.h"
#include <iostream>

ThreadManager::ThreadManager() {
    for (int i = 0; i < MAX_THREAD_NUM; i++)
    {
        minHeap.push(i);
    }
}

int ThreadManager::append_thread(void (*f)(void)){
    ThreadPointer thread_ptr (new Thread(this->get_first_available_id(), f));
    this->threads[thread_ptr->getId()] = thread_ptr;
    return thread_ptr->getId();
}

int ThreadManager::get_first_available_id() {
    int firstID = minHeap.top();
    minHeap.pop();
    return firstID;
}

std::ostream &operator<<(std::ostream &stream, const ThreadManager &thread_manager) {
    stream << "Printing Thread Manager's data" << std::endl;
    stream << "Threads: " << std::endl;

    for(const auto& thread: thread_manager.threads){
        stream << *thread.second;
    }
    return stream;
}

int ThreadManager::delete_thread(int id) {
    minHeap.push(id);
    threads.erase(id);
}



