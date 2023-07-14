#include "share_ptr.h"

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
    cout << "now counts:" << ptr2.use_count() << endl;
    {
        SharePtrTest w(ptr2);
        cout << "now counts:" << ptr2.use_count() << endl;
    }
    cout << "now counts:" << ptr2.use_count() << endl;

    cout << ptr2.use_count() << "end" << endl;
}

void Ptr_Test2()
{
    cout << "Ptr_Test2 start." << endl;
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
        cout << "ptr1 is alive.";
    }
    if (ptr2)
    {
        cout << "ptr2 is alive.";
    }
    if (ptr3)
    {
        cout << "ptr3 is alive.";
    }
    if (ptr4)
    {
        cout << "ptr4 is alive.";
    }
    cout << "Ptr_Test2 finish." << endl;
}

void Ptr_Test3()
{
    TestDataB *b = new TestDataB("xixixi");
    shared_ptr<TestData> v(b);
    // shared_ptr<TestData> v = make_shared<TestData>("xixixi");
    cout << v->getName()<<endl;
    cout << "Ptr_Test3 finish." << endl;
}

void showData(shared_ptr<TestData> _t)
{
    _t->setName("nininini");
    cout << "_t.count:"<<_t.use_count()<<",data:"<<_t->getName()<<endl;
}

void Ptr_Test4()
{
    shared_ptr<TestDataB> b = make_shared<TestDataB>("yayaya");
    cout << "b.count:"<<b.use_count()<<",data:"<<b->getName()<<endl;
    showData(dynamic_pointer_cast<TestData>(b));
    cout << "b.count:"<<b.use_count()<<",data:"<<b->getName()<<endl;
    cout << "b.count:"<<dynamic_pointer_cast<TestData>(b).use_count()<<",data:"<<b->getName()<<endl;
    cout << "Ptr_Test4 finish." << endl;
}

TestData::TestData(const string &_name)
{
    m_name = _name;
    // cout<<"\""<<m_name<<"\" had new."<<endl;
}

TestData::~TestData()
{
    cout << "\"" << m_name << "\" had delete." << endl;
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
    cout << "SharePtrTest count:" << _ptr.use_count() << endl;
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
    cout << "ptr1{" << m_ptr->getName() << "}, ptr2{" << m_ptr2->getName() << "," << m_ptr2.use_count() << endl;
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
