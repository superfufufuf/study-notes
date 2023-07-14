#include "threadTest.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

ThreadData::ThreadData()
{
    cout << __LINE__ << "ThreadData has create" << endl;
}

ThreadData::ThreadData(const std::string &_data)
{
    m_data = _data;
    cout << __LINE__ << "ThreadData has create. data:" << m_data << endl;
}

ThreadData::ThreadData(const ThreadData &_other)
{
    m_data = _other.m_data;
    cout << __LINE__ << "ThreadData has create. data:" << m_data << endl;
}

ThreadData::ThreadData(const ThreadData &&_other)
{
    m_data = std::move(_other.m_data);
    cout << __LINE__ << "ThreadData has create. data:" << m_data << endl;
}

ThreadData &ThreadData::operator=(const ThreadData &_other)
{
    if (this != &_other)
    {
        m_data = _other.m_data;
        cout << __LINE__ << "ThreadData has create. data:" << m_data << endl;
    }
    return *this;
}

ThreadData &ThreadData::operator=(const ThreadData &&_other)
{
    if (this != &_other)
    {
        m_data = std::move(_other.m_data);
        cout << __LINE__ << "ThreadData has create. data:" << m_data << endl;
    }
    return *this;
}

ThreadData::~ThreadData()
{
    cout << "ThreadData has delete. data:" << m_data << endl;
}

void ThreadA::CreateDataAndSend()
{
    ThreadData test("AAAA");
    if (m_fun)
    {
        thread(m_fun, test).detach();
    }
}

void ThreadA::SetCallBack(std::function<void(const ThreadData &)> _fun)
{
    m_fun = _fun;
}

void ThreadB::RecvData(const ThreadData &_data)
{
    this_thread::sleep_for(std::chrono::milliseconds(5000));
    cout << "recv data." << endl;
}
