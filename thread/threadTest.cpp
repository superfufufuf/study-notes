#include "threadTest.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "LogManager.h"

using namespace std;

ThreadData::ThreadData()
{
    _LOG(to_string(__LINE__) + "ThreadData has create", LogLevel::DEBUG);
}

ThreadData::ThreadData(const std::string &_data)
{
    m_data = _data;
    _LOG(to_string(__LINE__) + "ThreadData has create. data:" + m_data, LogLevel::DEBUG);
}

ThreadData::ThreadData(const ThreadData &_other)
{
    m_data = _other.m_data;
    _LOG(to_string(__LINE__) + "ThreadData has create. data:" + m_data, LogLevel::DEBUG);
}

ThreadData::ThreadData(const ThreadData &&_other)
{
    m_data = std::move(_other.m_data);
    _LOG(to_string(__LINE__) + "ThreadData has create. data:" + m_data, LogLevel::DEBUG);
}

ThreadData &ThreadData::operator=(const ThreadData &_other)
{
    if (this != &_other)
    {
        m_data = _other.m_data;
        _LOG(to_string(__LINE__) + "ThreadData has create. data:" + m_data, LogLevel::DEBUG);
    }
    return *this;
}

ThreadData &ThreadData::operator=(const ThreadData &&_other)
{
    if (this != &_other)
    {
        m_data = std::move(_other.m_data);
        _LOG(to_string(__LINE__) + "ThreadData has create. data:" + m_data, LogLevel::DEBUG);
    }
    return *this;
}

ThreadData::~ThreadData()
{
    _LOG("ThreadData has delete. data:" + m_data, LogLevel::DEBUG);
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
    _LOG("recv data.", LogLevel::DEBUG);
}
