#ifndef TASK_EXECUTOR_H
#define TASK_EXECUTOR_H

#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <future>
#include <functional>
#include <stdexcept>

using namespace std;

class TaskExecutor
{
public:
    template <typename Func, typename... Args>
    auto AddTask(Func &&_func, Args &&..._args)
        -> std::future<typename std::result_of<Func(Args...)>::type>;

protected:
    void DoTask(const int &&_taskNum = -1);

private:
    queue<function<void()>> m_taskList;
    mutex m_taskMtx;
};

template <typename Func, typename... Args>
auto TaskExecutor::AddTask(Func &&_func, Args &&..._args)
    -> std::future<typename std::result_of<Func(Args...)>::type>
{
    using return_type = typename std::result_of<Func(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<Func>(_func), std::forward<Args>(_args)...));

    std::future<return_type> res = task->get_future();
    {
        lock_guard<mutex> locker(m_taskMtx);
        m_taskList.emplace([task]()
                           { (*task)(); });
    }
    return res;
}

inline void TaskExecutor::DoTask(const int &&_taskNum)
{
    lock_guard<mutex> locker(m_taskMtx);

    int finalDoTaskNum = _taskNum;
    if (_taskNum < 0)
    {
        finalDoTaskNum = m_taskList.size();
    }

    for (int i = 0; i < finalDoTaskNum; i++)
    {
        if (m_taskList.size() > 0)
        {
            auto func = m_taskList.front();
            func();
            m_taskList.pop();
        }
        else
        {
            break;
        }
    }
}

#endif