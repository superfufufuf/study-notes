#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "TaskExecutor.hpp"

class TaskManager : public TaskExecutor
{
public:
    void Run();
};

#endif