//
// Created by amitroth on 4/23/23.
//

#include <memory>
#include <iostream>

#include "Thread.h"
#include "ThreadManager.h"

int main(){
    auto t1 = std::make_shared<Thread>(0, nullptr);
    auto t2 = std::make_shared<Thread>(1, nullptr);
    auto t3 = std::make_shared<Thread>(3, nullptr);

    auto manager = std::make_shared<ThreadManager>();
    manager->append_thread(t1);
    manager->append_thread(t2);
    manager->append_thread(t3);
    std::cout << manager << std::endl;


}
