#include "future_.h"
#include <future>
#include <chrono>
#include <functional>
#include <thread>

using namespace std;

string printFun()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1 * 1000));
    cout << "printFun had sleep over" << endl;
    return "printFun";
}

void threadTest(std::promise<string> &_promise)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(5 * 1000));
    cout << "threadTest had sleep over" << endl;
    _promise.set_value("threadTest");
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1 * 1000));
    }
}

FutureTest::~FutureTest()
{
    cout << "FutureTest had delete" << endl;
}

void FutureTest::test()
{
    std::future<string> fut = std::async(std::bind(printFun));
    cout << "fut had get:" << fut.get() << endl;

    std::promise<string> pro;
    std::future<string> fut2 = pro.get_future();
    thread thread1(threadTest, std::ref(pro));
    thread1.detach();
    fut2.wait();
    cout << "fut2 had get:" << fut2.get() << endl;

    cout << "future test ok" << endl;
}
