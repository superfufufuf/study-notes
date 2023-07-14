#include "TaskManager.h"
#include <thread>
#include <iostream>

void TaskManager::Run()
{
    cout << "Start TaskManager thread: " << this_thread::get_id() << endl;
    while(true)
    {
        DoTask();
        this_thread::sleep_for(chrono::milliseconds(1));
    }
}
