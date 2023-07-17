#include "future_.h"
#include <future>
#include <chrono>
#include <functional>
#include <thread>
#include "LogManager.h"

using namespace std;

string printFun()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1 * 1000));
    _LOG("printFun had sleep over", LogLevel::DEBUG);
    return "printFun";
}

void threadTest(std::promise<string> &_promise)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(5 * 1000));
    _LOG("threadTest had sleep over", LogLevel::DEBUG);
    _promise.set_value("threadTest");
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1 * 1000));
    }
}

FutureTest::~FutureTest()
{
    _LOG("FutureTest had delete", LogLevel::DEBUG);
}

void FutureTest::test()
{
    std::future<string> fut = std::async(std::bind(printFun));
    _LOG("fut had get:" + fut.get(), LogLevel::DEBUG);

    std::promise<string> pro;
    std::future<string> fut2 = pro.get_future();
    thread thread1(threadTest, std::ref(pro));
    thread1.detach();
    fut2.wait();
    _LOG("fut2 had get:" + fut2.get(), LogLevel::DEBUG);

    _LOG("future test ok", LogLevel::DEBUG);
}
