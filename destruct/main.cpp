#include "destruct.hpp"
#include <thread>
#include <chrono>

DestructChecker c1("globle");

int main(int argc, char **argv)
{
    DestructChecker c2("main");

    thread([]()
           {
               DestructChecker c3("thread");
               while (true)
               {
                   c1.checkAlive();
                   c3.checkAlive();
               }
           })
        .detach();

    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
    return 0;
}
