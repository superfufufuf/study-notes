#ifndef SHARE_PTR_H
#define SHARE_PTR_H

#include <memory>
#include <mutex>
#include <iostream>
#include <stdlib.h>
#include <list>

using namespace std;

void Ptr_Test1();
void Ptr_Test2();
void Ptr_Test3();
void Ptr_Test4();

class TestData
{
public:
    TestData()=delete;
    TestData(const string &_name);
    ~TestData();

    void setName(const string &_name);
    virtual string getName() const;

private:
    string m_name;
};

class TestDataB : public TestData
{
public:
    TestDataB(const string &_name);
    virtual string getName() const override;

private:
    string m_name;
};


class SharePtrTest
{
public:
    SharePtrTest();
    SharePtrTest(shared_ptr<TestData> _ptr);
    ~SharePtrTest();

    void setData(unique_ptr<TestData> &_ptr);
    void setData(shared_ptr<TestData> &_ptr);
    void printData() const;

private:
    void run();

    unique_ptr<TestData> m_ptr;
    shared_ptr<TestData> m_ptr2;
};

#endif