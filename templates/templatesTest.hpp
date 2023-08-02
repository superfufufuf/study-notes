#ifndef TEMPLATES_TEST_H
#define TEMPLATES_TEST_H

#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <memory>
#include <map>
#include <string.h>
#include "LogManager.h"

using namespace std;

class tempData
{
public:
    tempData(string _str)
    {
        m_data = _str;
    }

    string GetData() const
    {
        return m_data;
    }

private:
    string m_data;
};

template <typename T>
void ParseArgs(vector<shared_ptr<T>> &_list, shared_ptr<T> t)
{
    _list.push_back(t);
}

template <typename T, typename... Args>
void ParseArgs(vector<shared_ptr<T>> &_list, shared_ptr<T> t, Args... args)
{
    _list.push_back(t);
    ParseArgs(_list, forward<Args>(args)...);
}

class tempA
{
private:
    vector<shared_ptr<tempData>> strList;

public:
    template <typename T = tempData, typename... Args>
    tempA(shared_ptr<T> t, Args... args)
    {
        ParseArgs(strList, forward<shared_ptr<T>>(t), forward<Args>(args)...);
    }
    ~tempA()
    {
    }

    void print() const
    {
        bool isFirst = true;
        string info;
        for (auto item : strList)
        {
            if (!isFirst)
            {
                info += ",";
            }
            info += item->GetData();
            isFirst = false;
        }
        _LOG(info, LogLevel::DEBUG);
    }
};

template <typename _Map, typename Fun>
void map_remove_if(_Map &_map, Fun _fun)
{
    for (auto itor = _map.begin(); itor != _map.end();)
    {
        if (_fun(*itor))
        {
            itor = _map.erase(itor);
        }
        else
        {
            itor++;
        }
    }
}

template <typename, typename, size_t>
class TTestA;

template <typename R, typename B, size_t Q>
class TTestA
{
public:
    TTestA()
    {
    }

    static void print(R _r, B _b)
    {
        cout << "[TTestA]" << _r << "," << _b << ", n:" << Q << endl;
    }

private:
    char str[Q];
};

template <typename R, size_t Q>
class TTestA<R, int, Q>
{
public:
    TTestA()
    {
    }

    static void print(R _r, int _b)
    {
        cout << "[TTestA<R, int , Q>]" << _r << "," << _b << ", n:" << Q << endl;
    }

private:
    char str[Q];
};

template <template <typename K> typename T, typename R, size_t Q>
class TTestA<T<R>, int, Q>
{
public:
    TTestA()
    {
    }

    static void print(R _r, int _b)
    {
        ;
        cout << "[TTestA<T<R>, int, Q>]";
        for (auto value : _r)
        {
            cout << value << ",";
        }
        cout << "," << _b << ", n:" << Q << endl;
    }

private:
    char str[Q];
};

template <typename T>
void printArgs(T &&t)
{
    cout << t << endl;
}

template <typename T, typename... Args>
void printArgs(T &&t, Args &&...args)
{
    cout << t << ",";
    printArgs(forward<Args>(args)...);
}

template <class T>
typename std::enable_if<std::is_integral<T>::value, bool>::type is_odd(T i)
{
    return bool(i % 2);
}

template <typename T, typename F = typename std::enable_if<std::is_integral<T>::value, void>::type>
void PrintDatalength(const T data)
{
    cout << std::is_same<F, void>::value << endl;
    cout << std::is_same<F, bool>::value << endl;
    cout << std::is_same<F, uint8_t>::value << endl;
    cout << std::is_same<F, uint16_t>::value << endl;
    cout << std::is_same<F, int32_t>::value << endl;
    cout << "data[" << data << "]: " << sizeof(data) << " byte" << endl;
}

void TestTemplate()
{
    list<int> testList;
    testList.push_back(12);
    testList.push_back(14);
    cout << is_odd(231) << "," << is_odd(232) << endl;
    PrintDatalength(true);
    PrintDatalength('a');
    PrintDatalength(uint16_t(6000000));
    PrintDatalength(int32_t(425));

    TTestA<int, int, 10> ta;

    ta.print(1104, 2260);
    // TTestA<list<int>, string, 12>::print(testList, "11550");
    ta.print(21412, 3740155);
}

#endif