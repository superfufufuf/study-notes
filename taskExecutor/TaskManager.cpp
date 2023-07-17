#include "TaskManager.h"
#include <thread>
#include <iostream>
#include "LogManager.h"

void TaskManager::Run()
{
    auto threadId = this_thread::get_id();
    _LOG("Start TaskManager thread: " + to_string(*(unsigned int *)&threadId), LogLevel::DEBUG);
    while (true)
    {
        DoTask();
        this_thread::sleep_for(chrono::milliseconds(1));
    }
}
