#ifndef SIGNAL_SLOT_MANAGER_H
#define SIGNAL_SLOT_MANAGER_H

#include <iostream>
#include <optional>
#include <memory>
#include <mutex>
#include <functional>
#include <map>
#include <any>
#include <optional>

#include "SignalSlot.hpp"

class SignalSlotManager
{
public:
    static SignalSlotManager &GetInstance()
    {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]()
                       { m_instance.reset(new SignalSlotManager); });

        return *m_instance;
    }
    ~SignalSlotManager() = default;

    template <typename... Arg>
    std::function<void(Arg...)> getEmitSignal(uint32_t key)
    {
        std::optional<std::shared_ptr<SignalSlot<Arg...>>> signalSlot;
        auto iter = signalMap.find(key);
        if (iter != signalMap.end())
        {
            signalSlot = std::any_cast<std::shared_ptr<SignalSlot<Arg...>>>(iter->second);
        }
        else
        {
            signalSlot = std::make_optional<std::shared_ptr<SignalSlot<Arg...>>>(std::make_shared<SignalSlot<Arg...>>());
            std::any oneSignalSlot = signalSlot;
            signalMap.insert(make_pair(key, oneSignalSlot));
        }

        if constexpr (sizeof...(Arg) == 0)
        {
            return std::bind(&SignalSlot<Arg...>::Emit, *signalSlot);
        }
        else if constexpr (sizeof...(Arg) == 1)
        {
            return std::bind(&SignalSlot<Arg...>::Emit, *signalSlot, std::placeholders::_1);
        }
        else if constexpr (sizeof...(Arg) == 2)
        {
            return std::bind(&SignalSlot<Arg...>::Emit, *signalSlot, std::placeholders::_1, std::placeholders::_2);
        }
        else if constexpr (sizeof...(Arg) == 3)
        {
            return std::bind(&SignalSlot<Arg...>::Emit, *signalSlot, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        }
    }

    template <typename... Arg>
    void setSlotFunc(uint32_t key, std::function<void(Arg...)> func)
    {
        auto iter = signalMap.find(key);
        if (iter != signalMap.end())
        {
            auto signalSlot = std::any_cast<std::shared_ptr<SignalSlot<Arg...>>>(iter->second);
            signalSlot->Connect(func);
        }
        else
        {
            std::shared_ptr<SignalSlot<Arg...>> signalSlot = std::make_shared<SignalSlot<Arg...>>();
            signalSlot->Connect(func);
            std::any oneSignalSlot = signalSlot;
            signalMap.insert(make_pair(key, oneSignalSlot));
        }
    }

    template <typename... Arg>
    std::function<void(Arg...)> Connect(uint32_t key, std::function<void(Arg...)> func)
    {
        std::shared_ptr<SignalSlot<Arg...>> signalSlot;
        auto iter = signalMap.find(key);
        if (iter != signalMap.end())
        {
            signalSlot = std::any_cast<std::shared_ptr<SignalSlot<Arg...>>>(iter->second);
            signalSlot->Connect(func);
        }
        else
        {
            signalSlot = std::make_shared<SignalSlot<Arg...>>();
            signalSlot->Connect(func);
            std::any oneSignalSlot = signalSlot;
            signalMap.insert(make_pair(key, oneSignalSlot));
        }

        if constexpr (sizeof...(Arg) == 0)
        {
            return std::bind(&SignalSlot<Arg...>::Emit, *signalSlot);
        }
        else if constexpr (sizeof...(Arg) == 1)
        {
            return std::bind(&SignalSlot<Arg...>::Emit, *signalSlot, std::placeholders::_1);
        }
        else if constexpr (sizeof...(Arg) == 2)
        {
            return std::bind(&SignalSlot<Arg...>::Emit, *signalSlot, std::placeholders::_1, std::placeholders::_2);
        }
        else if constexpr (sizeof...(Arg) == 3)
        {
            return std::bind(&SignalSlot<Arg...>::Emit, *signalSlot, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        }
    }

private:
    std::map<uint32_t, std::any> signalMap;

private:
    SignalSlotManager();
    SignalSlotManager(const SignalSlotManager &) = delete;
    SignalSlotManager &operator=(const SignalSlotManager &) = delete;

    static std::unique_ptr<SignalSlotManager> m_instance;
};

std::unique_ptr<SignalSlotManager> SignalSlotManager::m_instance;

#define SignalSlotManagerInstance SignalSlotManager::GetInstance()

#endif