#ifndef RVV_TEST_H
#define RVV_TEST_H

#include <iostream>
#include <string>

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
    cout << __LINE__ << "RRTestData has create" << endl;
}

RRTestData::RRTestData(const std::string &_data)
{
    m_data = _data;
    cout << "RRTestData(const std::string &_data) " << "RRTestData has create. data:" << m_data << endl;
}

RRTestData::RRTestData(const RRTestData &_other)
{
    m_data = _other.m_data;
    cout << "RRTestData(const RRTestData &_other) " << "RRTestData has create. data:" << m_data << endl;
}

RRTestData::RRTestData(const RRTestData &&_other)
{
    m_data = std::move(_other.m_data);
    cout << "RRTestData(const RRTestData &&_other) " << "RRTestData has create. data:" << m_data << endl;
}

RRTestData &RRTestData::operator=(const RRTestData &_other)
{
    if (this != &_other)
    {
        m_data = _other.m_data;
        cout << "operator=(const RRTestData &_other) " << "RRTestData has create. data:" << m_data << endl;
    }
    return *this;
}

RRTestData &RRTestData::operator=(const RRTestData &&_other)
{
    if (this != &_other)
    {
        m_data = std::move(_other.m_data);
        cout << "operator=(const RRTestData &&_other) " << "RRTestData has create. data:" << m_data << endl;
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
    cout << "RRTestData has delete. data:" << m_data << endl;
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
    cout << _rRData.GetData() << endl;
    _rRData.SetData(_rRData.GetData()+" had show");
}

void TestRR()
{
    RRTestData tData1("gagagaga");
    ShowData(move(tData1));
    cout << tData1.GetData() << endl;
    ShowData(RRTestData("hahahaha"));
    RRTestData tData2("giaogiaogiao");
}

#endif