#ifndef THREAD_TEST_H
#define THREAD_TEST_H

#include <string>
#include <functional>

class ThreadData
{
public:
    ThreadData();
    ThreadData(const std::string &_data);
    ThreadData(const ThreadData &_other);
    ThreadData(const ThreadData &&_other);
    ThreadData& operator=(const ThreadData &_other);
    ThreadData& operator=(const ThreadData &&_other);

    ~ThreadData();


private:
    std::string m_data;
};

class ThreadA
{
public:
    void CreateDataAndSend();

    void SetCallBack(std::function<void(const ThreadData &)> _fun);

private:
    std::function<void(const ThreadData &)> m_fun;
};

class ThreadB
{
public:
    void RecvData(const ThreadData &_data);
};

#endif