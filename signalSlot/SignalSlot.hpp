#ifndef SignalSlotSLOT_H
#define SignalSlotSLOT_H

#include <list>
#include <iostream>
#include <functional>

template <typename... Args>
class SignalSlot
{
public:
    void Connect(std::function<void(Args...)> _callback)
    {
        m_callBackList.push_back(_callback);
    }

    void DeConnect(std::function<void(Args...)> _callback)
    {
        m_callBackList.remove_if([&](std::function<void(Args...)> func)
                                 { return (_callback.target_type() == func.target_type()); });
    }

    void Emit(Args... args)
    {
        for (auto callBack : m_callBackList)
        {
            callBack(args...);
        }
    }

private:
    std::list<std::function<void(Args...)>> m_callBackList;
};

void SSA(int i)
{
    std::cout << "exec ASlot " << i << std::endl;
}

class SSB
{
public:
    void BSlot(int i)
    {
        std::cout << "exec BSlot[" << id << "] " << i << std::endl;
    }

private:
    int id = 12;
};

void SSC(int i, float j)
{
    std::cout << "exec CSlot " << i << ", " << j << std::endl;
}

void TestSignalSlot()
{
    SignalSlot<int> x;
    SignalSlot<int, float> y;

    {
        SSB *b = new SSB();

        x.Connect(std::bind(SSA, std::placeholders::_1));
        x.Connect(std::bind(&SSB::BSlot, b, std::placeholders::_1));
        x.Emit(10);

        x.DeConnect(std::bind(&SSB::BSlot, b, std::placeholders::_1));
        delete b;
    }
    x.Emit(11);

    y.Connect(SSC);
    y.Emit(19, 2.56);
}

#endif
