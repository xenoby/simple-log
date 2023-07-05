#include <iostream>
#include <thread>

#include "simple-log.hxx"

void task_one();
void task_two();

int main()
{
    
    std::thread thread_a(task_one);
    std::thread thread_b(task_two);

    unsigned i = 1;
    
    while(true)
    {   
        tools::xout.log(NONE) << "somebody message! just for fun"; Sleep(360 * i);
        tools::xout.log(LOG) << "LOG!"; Sleep(360 * i);
        i++;
        tools::xout.log(DEBUG) << "DEBUG!"; Sleep(360 * i);
        i++;
        tools::xout.log(INFO) << "INFO!"; Sleep(360 * i);
        i++;
        tools::xout.log(WARNING) << "WARNING!"; Sleep(360 * i);
        i++;
        tools::xout.log(CRITICAL) << "CRITICAL!"; Sleep(360 * i);
        i++;
        tools::xout.log(ERRORS) << "ERRORS!"; Sleep(360 * i);
        i = 1;
    }

    thread_a.join();
    thread_b.join();

    return 0;
}

void task_one()
{
    while (true)
    {
        tools::xout.log(NONE) << "somebody message from thread one!";
        std::this_thread::sleep_for(std::chrono::seconds(9));
    }
    
}

void task_two()
{
    while (true)
    {
        tools::xout.log(LOG) << "thread two is working!"; 
        std::this_thread::sleep_for(std::chrono::seconds(18));
    }
}
