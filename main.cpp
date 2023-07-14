#include "const.h"
#include "share_ptr.h"
#include "future_.h"
#include "threadTest.h"
#include "RvalueReference.hpp"
#include "TaskManager.h"
#include "templatesTest.hpp"
#include "MyWindow.h"
#include "destruct.hpp"

using namespace std;

#define TEST_ID 8

std::string Fun1(const std::string &_str, const int _index)
{
    cout << "fun[" << __func__ << "] thread: " << this_thread::get_id() << endl;
    this_thread::sleep_for(chrono::milliseconds(2000));
    return _str + "," + to_string(_index);
}

int main(int argc, char const *argv[])
{
    cout << "main thread: " << this_thread::get_id() << endl;

#if TEST_ID == 1
    Ptr_Test1();
    Ptr_Test2();
    Ptr_Test3();
    Ptr_Test4();
#elif TEST_ID == 2
    FutureTest fut;
    fut.test();
#elif TEST_ID == 3
    ThreadA A;
    ThreadB B;

    A.SetCallBack(std::bind(&ThreadB::RecvData, B, placeholders::_1));
    A.CreateDataAndSend();
#elif TEST_ID == 4
    TestRR();
#elif TEST_ID == 5
    TaskManager taskManager;
    thread(&TaskManager::Run, &taskManager).detach();
    auto taskFuture = taskManager.AddTask(Fun1, "sadd", 21);
    taskFuture.wait();
    cout << "get future data:" << taskFuture.get() << endl;
#elif TEST_ID == 6
    shared_ptr<tempData> stra = make_shared<tempData>("AAA");
    shared_ptr<tempData> strb = make_shared<tempData>("BBB");
    shared_ptr<tempData> strc = make_shared<tempData>("CCC");
    tempA ta(stra);
    ta.print();

    map<string, tempData *> testMap;
    testMap.insert(make_pair("aaa", stra.get()));
    testMap.insert(make_pair("bbb", strb.get()));
    testMap.insert(make_pair("ccc", strc.get()));

    map_remove_if(testMap, [&](pair<string, tempData *> itor)
                  { return (itor.first == "aaa"); });
    cout << "testMap.size:" << testMap.size() << endl;
#elif TEST_ID == 7
    MyWindow myWindow;
    thread(std::bind(&MyWindow::Start, &myWindow)).detach();
#elif TEST_ID == 8
    TestDestruct();
#elif TEST_ID == 9
    int i = 10;
    float j = 1.234;
    cout << i <<","<< std::hex << std::showbase <<std::showpoint 
    << i << ","<<j<<endl;
#endif

    cout << "all things has done." << endl;
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(2*1000));
    }
    return 0;
}
