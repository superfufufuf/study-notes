#include "share_ptr.h"
#include "LogManager.h"

void Ptr_Test1()
{
    unique_ptr<TestData> ptr = make_unique<TestData>("gagaga");

    shared_ptr<TestData> ptr2 = make_shared<TestData>("hahaha");

    {
        SharePtrTest w;
        w.setData(ptr);
        w.setData(ptr2);
        w.printData();
    }
    _LOG("now counts:" + to_string(ptr2.use_count()), LogLevel::DEBUG);
    {
        SharePtrTest w(ptr2);
        _LOG("now counts:" + to_string(ptr2.use_count()), LogLevel::DEBUG);
    }
    _LOG("now counts:" + to_string(ptr2.use_count()), LogLevel::DEBUG);

    _LOG(to_string(ptr2.use_count()) + "end", LogLevel::DEBUG);
}

void Ptr_Test2()
{
    _LOG("Ptr_Test2 start.", LogLevel::DEBUG);
    shared_ptr<TestData> ptr1 = make_shared<TestData>("ptr1");
    ptr1.reset();
    shared_ptr<TestData> ptr2 = make_shared<TestData>("ptr2");
    ptr2.reset();
    unique_ptr<TestData> ptr3 = make_unique<TestData>("ptr3");
    ptr3.release();
    unique_ptr<TestData> ptr4 = make_unique<TestData>("ptr4");
    ptr4.release();
    if (ptr1)
    {
        _LOG("ptr1 is alive.", LogLevel::DEBUG);
    }
    if (ptr2)
    {
        _LOG("ptr2 is alive.", LogLevel::DEBUG);
    }
    if (ptr3)
    {
        _LOG("ptr3 is alive.", LogLevel::DEBUG);
    }
    if (ptr4)
    {
        _LOG("ptr4 is alive.", LogLevel::DEBUG);
    }
    _LOG("Ptr_Test2 finish.", LogLevel::DEBUG);
}

void Ptr_Test3()
{
    TestDataB *b = new TestDataB("xixixi");
    shared_ptr<TestData> v(b);
    // shared_ptr<TestData> v = make_shared<TestData>("xixixi");
    _LOG(v->getName(), LogLevel::DEBUG);
    _LOG("Ptr_Test3 finish.", LogLevel::DEBUG);
}

void showData(shared_ptr<TestData> _t)
{
    _t->setName("nininini");
    _LOG("_t.count:" + to_string(_t.use_count()) + ",data:" + _t->getName(), LogLevel::DEBUG);
}

void Ptr_Test4()
{
    shared_ptr<TestDataB> b = make_shared<TestDataB>("yayaya");
    _LOG("b.count:" + to_string(b.use_count()) + ",data:" + b->getName(), LogLevel::DEBUG);
    showData(dynamic_pointer_cast<TestData>(b));
    _LOG("b.count:" + to_string(b.use_count()) + ",data:" + b->getName(), LogLevel::DEBUG);
    _LOG("b.count:" + to_string(dynamic_pointer_cast<TestData>(b).use_count()) + ",data:" + b->getName(), LogLevel::DEBUG);
    _LOG("Ptr_Test4 finish.", LogLevel::DEBUG);
}

TestData::TestData(const string &_name)
{
    m_name = _name;
}

TestData::~TestData()
{
    _LOG("\"" + m_name + "\" had delete.", LogLevel::DEBUG);
}

void TestData::setName(const string &_name)
{
    m_name = _name;
}

string TestData::getName() const
{
    return m_name;
}

SharePtrTest::SharePtrTest()
{
}

SharePtrTest::SharePtrTest(shared_ptr<TestData> _ptr)
    : m_ptr2(_ptr)
{
    _LOG("SharePtrTest count:" + to_string(_ptr.use_count()), LogLevel::DEBUG);
}

SharePtrTest::~SharePtrTest()
{
}

void SharePtrTest::setData(unique_ptr<TestData> &_ptr)
{
    m_ptr = std::move(_ptr);
}

void SharePtrTest::setData(shared_ptr<TestData> &_ptr)
{
    m_ptr2 = _ptr;
}

void SharePtrTest::printData() const
{
    _LOG("ptr1{" + m_ptr->getName() + "}, ptr2{" + m_ptr2->getName() + "," + to_string(m_ptr2.use_count()), LogLevel::DEBUG);
}

void SharePtrTest::run()
{
}

TestDataB::TestDataB(const string &_name)
    : TestData(_name)
{
}

string TestDataB::getName() const
{
    return "TestDataB," + TestData::getName();
}
