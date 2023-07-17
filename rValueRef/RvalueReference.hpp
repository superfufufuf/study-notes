#ifndef RVV_TEST_H
#define RVV_TEST_H

#include <iostream>
#include <string>
#include "LogManager.h"

using namespace std;

class RRTestData
{
public:
    RRTestData();
    RRTestData(const std::string &_data);
    RRTestData(const RRTestData &_other);
    RRTestData(const RRTestData &&_other);
    RRTestData &operator=(const RRTestData &_other);
    RRTestData &operator=(const RRTestData &&_other);

    RRTestData operator+(const RRTestData &_other);

    ~RRTestData();

    void SetData(const string &_str);

    string GetData() const;

private:
    std::string m_data;
};

RRTestData::RRTestData()
{
    _LOG(__LINE__ + "RRTestData has create" , LogLevel::DEBUG);
}

RRTestData::RRTestData(const std::string &_data)
{
    m_data = _data;
    _LOG("RRTestData(const std::string &_data) RRTestData has create. data:" + m_data , LogLevel::DEBUG);
}

RRTestData::RRTestData(const RRTestData &_other)
{
    m_data = _other.m_data;
    _LOG("RRTestData(const RRTestData &_other) RRTestData has create. data:" + m_data , LogLevel::DEBUG);
}

RRTestData::RRTestData(const RRTestData &&_other)
{
    m_data = std::move(_other.m_data);
    _LOG("RRTestData(const RRTestData &&_other) RRTestData has create. data:" + m_data , LogLevel::DEBUG);
}

RRTestData &RRTestData::operator=(const RRTestData &_other)
{
    if (this != &_other)
    {
        m_data = _other.m_data;
        _LOG("operator=(const RRTestData &_other) RRTestData has create. data:" + m_data , LogLevel::DEBUG);
    }
    return *this;
}

RRTestData &RRTestData::operator=(const RRTestData &&_other)
{
    if (this != &_other)
    {
        m_data = std::move(_other.m_data);
        _LOG("operator=(const RRTestData &&_other) RRTestData has create. data:" + m_data , LogLevel::DEBUG);
    }
    return *this;
}

inline RRTestData RRTestData::operator+(const RRTestData &_other)
{
    RRTestData newData(m_data + _other.m_data);
    return newData;
}

RRTestData::~RRTestData()
{
    _LOG("RRTestData has delete. data:" + m_data , LogLevel::DEBUG);
}

inline void RRTestData::SetData(const string &_str)
{
    m_data = _str;
}

inline string RRTestData::GetData() const
{
    return m_data;
}

void ShowData(RRTestData &&_rRData)
{
    _LOG(_rRData.GetData() , LogLevel::DEBUG);
    _rRData.SetData(_rRData.GetData()+" had show");
}

void TestRR()
{
    RRTestData tData1("gagagaga");
    ShowData(move(tData1));
    _LOG(tData1.GetData() , LogLevel::DEBUG);
    ShowData(RRTestData("hahahaha"));
    RRTestData tData2("giaogiaogiao");
}

#endif