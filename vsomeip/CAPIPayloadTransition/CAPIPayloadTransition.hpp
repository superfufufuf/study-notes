#ifndef CAPIPayloadTransition_H
#define CAPIPayloadTransition_H

#include <vsomeip/vsomeip.hpp>

template <typename T>
void SomeIpByteVectorPushData(std::vector<vsomeip::byte_t> &byteVector, const T data)
{
    if (sizeof(data) == 1)
    {
        byteVector.push_back(data);
    }
    else if (sizeof(data) == 2)
    {
        byteVector.push_back((data >> 8) & 0xFF);
        byteVector.push_back(data & 0xFF);
    }
    else if (sizeof(data) == 4)
    {
        byteVector.push_back((data >> 24) & 0xFF);
        byteVector.push_back((data >> 16) & 0xFF);
        byteVector.push_back((data >> 8) & 0xFF);
        byteVector.push_back(data & 0xFF);
    }
    else
    {
        std::cout << sizeof(data) << std::endl;
    }
}

int32_t GetInt32Data(const std::vector<vsomeip::byte_t> &byteVector, int StartIndex)
{
    int data = 0;
    data += byteVector[StartIndex] << 24;
    data += byteVector[StartIndex + 1] << 16;
    data += byteVector[StartIndex + 2] << 8;
    data += byteVector[StartIndex + 3];

    return data;
}

#endif