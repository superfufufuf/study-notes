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
template <typename T>
T TempVerb{};

// a test data***********************************************************************************************
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

// to push a series of data in vector***********************************************************************************************
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

// map's remove_if***********************************************************************************************
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

// to test specialization***********************************************************************************************
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

// test arg...***********************************************************************************************
template <typename... Args>
auto multiAdd(Args &&...args)
{
    // unary right fold
    return (args + ...);
}

template <typename T>
void printArgs1(T &&t)
{
    cout << t << endl;
}

template <typename T, typename... Args>
void printArgs1(T &&t, Args &&...args)
{
    cout << t << ",";
    printArgs1(forward<Args>(args)...);
}

template <typename... Args>
void printArgs2(Args &&...args)
{
    ([&]()
     { cout << args;
    if constexpr (sizeof...(args) > 1)
    {
        cout << ",";
    } }(),
     ...);
    cout << endl;
}

// enable_if***********************************************************************************************
template <class T>
typename std::enable_if<std::is_integral<T>::value, bool>::type is_odd(T i)
{
    return bool(i % 2);
}

template <class T>
typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type to_string1(T _str)
{
    return _str;
}
template <class T>
typename std::enable_if<!std::is_same<T, std::string>::value, std::string>::type to_string1(T _str)
{
    return std::to_string(_str);
}

template <class T>
typename std::string to_string2(T _str)
{
    if constexpr (is_same<T, std::string>::value)
    {
        return _str;
    }
    else
    {
        // if don't use if constexpr, to_string2(std::string()) will error
        return std::to_string(_str);
    }
}

// test data length***********************************************************************************************
template <typename T, typename F = typename std::enable_if<std::is_integral<T>::value, T>::type>
void PrintDatalength(const T data)
{
    cout << "data[" << data << "]: ";
    if (std::is_same<F, void>::value)
    {
        cout << "type is void";
    }
    else if (std::is_same<F, bool>::value)
    {
        cout << "type is  bool";
    }
    else if (std::is_same<F, char>::value)
    {
        cout << "type is  char";
    }
    else if (std::is_same<F, uint8_t>::value)
    {
        cout << "type is  uint8_t";
    }
    else if (std::is_same<F, uint16_t>::value)
    {
        cout << "type is  uint16_t";
    }
    else if (std::is_same<F, int32_t>::value)
    {
        cout << "type is  int32_t";
    }
    else
    {
        cout << "type is  other";
    }
    cout << ", size is " << sizeof(data) << " byte" << endl;
}

void TestTemplate()
{
    list<int> testList;
    testList.push_back(12);
    testList.push_back(14);
    cout << is_odd(231) << "," << is_odd(232) << "," << multiAdd(10, 20, 30, 40) << endl;
    PrintDatalength(true);
    PrintDatalength('a');
    PrintDatalength(uint16_t(6000000));
    PrintDatalength(int32_t(425));

    TempVerb<float> = 15.6f;
    TempVerb<int> = 18;
    printArgs1("printArgs1", TempVerb<int>, TempVerb<float>, 'c');
    printArgs2("printArgs2", 12, 15.7, 'c');
    using YT = decltype(std::declval<int>());

    TTestA<int, int, 10> ta;
    ta.print(1104, 2260);
    // TTestA<list<int>, string, 12>::print(testList, "11550");
    ta.print(21412, 3740155);
}

#endif