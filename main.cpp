#include <dlfcn.h>
#include <any>
#include <optional>
#include <variant>

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
#include "SignalSlot.hpp"
#include "ping.hpp"
#include "randTest.h"

using namespace std;

typedef int (*operationFunc)(int, int);

#define TEST_ID_Ptr_Test 1
#define TEST_ID_Future_Test 2
#define TEST_ID_Thread_Test 3
#define TEST_ID_RValue_Test 4
#define TEST_ID_TaskManager_Test 5
#define TEST_ID_Template_Test 6
#define TEST_ID_Window_Test 7
#define TEST_ID_Ping_Test 8
#define TEST_ID_cout_Test 9
#define TEST_ID_library_Test 10
#define TEST_ID_Any 11
#define TEST_ID_Signal_Slot 12
#define TEST_ID_Rand 13

#define TEST_ID TEST_ID_Rand

class TwoIntData
{
public:
    TwoIntData(int _x, int _y)
        : x(_x), y(_y)
    {
        cout << "TwoIntData create." << endl;
    }

    ~TwoIntData()
    {
        cout << "TwoIntData[" << x << "," << y << "] delete." << endl;
    }

    void set(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    void print()
    {
        cout << "x:" << x << ", y:" << y << endl;
    }

    friend ostream &operator<<(ostream &os, TwoIntData &t)
    {
        os << "[" << t.x << "," << t.y << "]";
        return os;
    }

    int x;

private:
    int y;
};

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
    _LOG("testMap.size:" + to_string(testMap.size()), LogLevel::INFO);

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
#elif TEST_ID == TEST_ID_Ping_Test
    string pingIp;
    cout << "Please input ping IP:" << endl;
    cin >> pingIp;
    if (ping(pingIp.c_str(), 10000))
    {
        cout << "Ping succeed!" << endl;
    }
    else
    {
        cout << "Ping wrong!" << endl;
    }
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
    { // option test
        TwoIntData t1(1, 2);
        std::optional<TwoIntData> o1;
        cout << "first option." << endl;
        if (o1.has_value())
        {
            o1->print();
        }
        o1 = t1;
        cout << "second option." << endl;
        if (o1.has_value())
        {
            o1->print();
            t1.set(1001, 1002);
            o1->print();
        }
        std::optional<TwoIntData> o3{TwoIntData(3, 4)};
        cout << "third option." << endl;
        if (o3.has_value())
        {
            o3->print();
        }
        std::optional<TwoIntData> o4{in_place, 5, 6};
        cout << "fourth option." << endl;
        if (o4.has_value())
        {
            o4->print();
        }
        auto o5 = std::make_optional<TwoIntData>(7, 8);
        cout << "fifth option." << endl;
        if (o5.has_value())
        {
            o5->print();
        }
    }

    { // variant
        variant<int, TwoIntData, float> var1;
        var1 = 1.2f;                                           // 直接用=初始化
        cout << "the variant is " << get<float>(var1) << endl; // 1.2
        try
        {
            get<int>(var1); // var1 含 float 而非 int ：将抛出
        }
        catch (const std::bad_variant_access &)
        {
            cout << "get error." << endl;
        }
        // cout<<"the variant is "<<get<int>(var1)<<endl;//std::bad_variant_access
        var1 = 2;
        cout << "the variant is " << get<int>(var1) << endl; // 2
        cout << "the variant is " << get<0>(var1) << endl;   // get<0>(...) == get<int>(...)

        variant<int, TwoIntData, float> var2(in_place_type<TwoIntData>, 100, 200);
        cout << "the variant is " << get<TwoIntData>(var2) << endl; // std::get<>()

        variant<int, TwoIntData, float> var3(in_place_index<1>, TwoIntData(300, 400)); // move
        cout << "the variant is " << get<1>(var3) << endl;

        var3.emplace<2>(1.2f);
        var3.emplace<1>(500, 600);                         // 这里注意与optional的emplace的区别，需要加定位
        cout << "the variant is " << get<1>(var3) << endl; // 2
        cout << var3.index() << endl;                      // index == 1

        using var_t = std::variant<int, long, double, std::string>;
        std::vector<var_t> vec = {10, 15l, 1.5, "hello"};
        for (auto v : vec)
        {
            visit([](auto &&args)
                  {
                    using m_type = decay_t<decltype(args)>; //remove cv
                    if constexpr (is_same_v<m_type, int>)
                    {
                        cout << "data[" << args << "] type is int" << endl;
                    } // if后跟constexpr为编译时判断
                    else if constexpr (is_same_v<m_type, long>)
                    {
                        cout << "data[" << args << "] type is long" << endl;
                    }
                    else if constexpr (is_same_v<m_type, double>)
                    {
                        cout << "data[" << args << "] type is double" << endl;
                    }
                    else if constexpr (is_same_v<m_type, string>)
                    {
                        cout << "data[" << args << "] type is string" << endl;
                    } },
                  v);
        }
    }

    { // any
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

        std::any b;
        {
            shared_ptr<TwoIntData> i = make_shared<TwoIntData>(10, 20);
            b = i;
            (*i).set(11, 21);
            cout << std::any_cast<shared_ptr<TwoIntData>>(b).use_count() << endl;
        }
        cout << std::any_cast<shared_ptr<TwoIntData>>(b).use_count() << endl;
        cout << b.type().name() << " " << std::any_cast<shared_ptr<TwoIntData>>(b)->x << endl;
    }
#elif TEST_ID == TEST_ID_Signal_Slot
    TestSignalSlot();
#elif TEST_ID == TEST_ID_Rand
    testRand();
    testRandom();
#endif

    _LOG("all things has done.", LogLevel::INFO);
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(2 * 1000));
    }
    return 0;
}
