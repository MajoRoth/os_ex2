//
// Created by amitroth on 4/23/23.
//

#include <memory>
#include <iostream>

#include "Thread.h"
#include "ThreadManager.h"

int main(){

    auto manager = std::make_shared<ThreadManager>();
    auto managerr = new ThreadManager();
    manager->append_thread(nullptr);
    manager->append_thread(nullptr);
    manager->append_thread(nullptr);
    std::cout << *manager << std::endl;


}
