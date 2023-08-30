#include <dlfcn.h>
#include <any>

#include "const.h"
#include "share_ptr.h"
#include "future_.h"
#include "threadTest.h"
#include "RvalueReference.hpp"
#include "TaskManager.h"
#include "templatesTest.hpp"
#include "MyWindow.h"
#include "MyEGLClass.h"
#include "destruct.hpp"
#include "LogManager.h"

using namespace std;

typedef int (*operationFunc)(int, int);

#define TEST_ID_Ptr_Test 1
#define TEST_ID_Future_Test 2
#define TEST_ID_Thread_Test 3
#define TEST_ID_RValue_Test 4
#define TEST_ID_TaskManager_Test 5
#define TEST_ID_Template_Test 6
#define TEST_ID_Window_Test 7
#define TEST_ID_Destruct_Test 8
#define TEST_ID_cout_Test 9
#define TEST_ID_library_Test 10
#define TEST_ID_Any 11

#define TEST_ID TEST_ID_Any

std::string Fun1(const std::string &_str, const int _index)
{
    auto threadId = this_thread::get_id();
    _LOG("fun[" + string(__func__) + "] thread: " + to_string(*(unsigned int *)&threadId), LogLevel::INFO);
    this_thread::sleep_for(chrono::milliseconds(2000));
    return _str + "," + to_string(_index);
}

int main(int argc, char const *argv[])
{
    LogManager::GetInstance().SetWriteLogLevel(LogLevel::ALL);
    LogManager::GetInstance().SetWriteLogMethod(WriteLogMethod::CONSOLE);
    LogManager::GetInstance().SetTimeFormat("%H:%M:%S");
    LogManager::GetInstance().SetLogPostionShow(LogPosShow_Func | LogPosShow_Line);

#if TEST_ID == TEST_ID_Ptr_Test
    Ptr_Test1();
    Ptr_Test2();
    Ptr_Test3();
    Ptr_Test4();
#elif TEST_ID == TEST_ID_Future_Test
    FutureTest fut;
    fut.test();
#elif TEST_ID == TEST_ID_Thread_Test
    ThreadA A;
    ThreadB B;

    A.SetCallBack(std::bind(&ThreadB::RecvData, B, placeholders::_1));
    A.CreateDataAndSend();
#elif TEST_ID == TEST_ID_RValue_Test
    TestRR();
#elif TEST_ID == TEST_ID_TaskManager_Test
    TaskManager taskManager;
    thread(&TaskManager::Run, &taskManager).detach();
    auto taskFuture = taskManager.AddTask(Fun1, "sadd", 21);
    taskFuture.wait();
    _LOG("get future data:" + taskFuture.get(), LogLevel::INFO);
#elif TEST_ID == TEST_ID_Template_Test
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
    _LOG("testMap.size:" << testMap.size(), LogLevel::INFO);

    TestTemplate();
#elif TEST_ID == TEST_ID_Window_Test
    auto threadId = this_thread::get_id();
    _LOG("main thread: " + to_string(*(unsigned int *)&threadId), LogLevel::INFO);
    Rect windowRect(0, 0, 600, 400);
    unsigned long displayId = 0;
    unsigned long windowId = 0;
#ifdef _FOR_X11_
    MyX11Class window(windowRect);
    displayId = (unsigned long)window.GetDisplay();
    windowId = (unsigned long)window.GetWindow();
    window.Start();
#endif
    // MyEGLClass myEgl(displayId, windowId, windowRect);
    // thread(&MyEGLClass::Render, &myEgl).detach();
    thread([&]()
           {
        MyEGLClass myEgl(displayId, windowId, windowRect);
        myEgl.Render(); })
        .detach();
#elif TEST_ID == TEST_ID_Destruct_Test
    TestDestruct();
#elif TEST_ID == TEST_ID_cout_Test
    int i = 10;
    float j = 1.234;
    cout << i << "," << std::hex << std::showbase << std::showpoint
         << i << "," << j << endl;
#elif TEST_ID == TEST_ID_library_Test
    auto handle = dlopen("libmylibrary.so", RTLD_LAZY | RTLD_GLOBAL);
    auto errorCode = dlerror();
    if (errorCode == nullptr)
    {
        // 符号表: nm -D libmylibrary.so
        // 00000000000010f9 T addTest
        //                 w __cxa_finalize
        //                 w __gmon_start__
        //                 w _ITM_deregisterTMCloneTable
        //                 w _ITM_registerTMCloneTable
        // 000000000000115c T mulTest
        // 0000000000001111 T subTest
        // 000000000000113e T _Z7divTestff
        // 0000000000001127 T _Z7divTestii

        char addTest[] = "addTest";
        char subTest[] = "subTest";
        char mulTest[] = "mulTest";
        char divTest[] = "_Z7divTestii";
        char *test = divTest;
        auto funcSympol = dlsym(handle, test);
        errorCode = dlerror();
        if (errorCode == nullptr)
        {
            auto func = reinterpret_cast<operationFunc>(funcSympol);
            cout << "func{" << test << "}, arg{1000, 100}, value: " << (*func)(1000, 100) << endl;
        }
        else
        {
            cout << errorCode << endl;
        }
    }
    else
    {
        cout << errorCode << endl;
    }

#elif TEST_ID == TEST_ID_Any
    std::any a = 1;
    cout << a.type().name() << " " << std::any_cast<int>(a) << endl;
    a = 2.2f;
    cout << a.type().name() << " " << std::any_cast<float>(a) << endl;
    if (a.has_value())
    {
        cout << "a has value" << endl;
    }
    a.reset();
    if (a.has_value())
    {
        cout << "a has value" << endl;
    }
    a = std::string("a");
    cout << a.type().name() << " " << std::any_cast<std::string>(a) << endl;
#endif

    _LOG("all things has done.", LogLevel::INFO);
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(2 * 1000));
    }
    return 0;
}
