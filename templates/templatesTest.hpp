#ifndef TEMPLATES_TEST_H
#define TEMPLATES_TEST_H

#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <memory>
#include <map>

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
        for (auto item : strList)
        {
            if (!isFirst)
            {
                cout << ",";
            }
            cout << item->GetData();
            isFirst = false;
        }
        cout << endl;
    }
};

template<typename _Map, typename Fun>
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

#endif