#include <thread>
#include <iostream>
#include <unistd.h>
#include <CommonAPI/CommonAPI.hpp>
#include <v1/commonapi/examplesA/CAPITestAProxy.hpp>
#include "ModelA.h"
#include "CAPIConst.h"

#define TEST_ASYNC 0

using namespace v1_2::commonapi::examplesA;

#if TEST_ASYNC
void recv_cb(const CommonAPI::CallStatus &callStatus,
             const CAPITestA::stdErrorTypeEnum &methodError,
             const int32_t &y1,
             const std::string &y2)
{
    std::cout << "Result of asynchronous call of foo: " << std::endl;
    std::cout << "   callStatus: " << ((callStatus == CommonAPI::CallStatus::SUCCESS) ? "SUCCESS" : "NO_SUCCESS")
              << std::endl;
    std::cout << "   error: "
              << ((methodError == CAPITestA::stdErrorTypeEnum::NO_FAULT) ? "NO_FAULT" : "MY_FAULT") << std::endl;
    std::cout << "   Output values: y1 = " << y1 << ", y2 = " << y2 << std::endl;
}
#endif

void recv_cb(const CommonAPI::CallStatus &callStatus, const int32_t &val)
{
    std::cout << "Receive callback: " << val << std::endl;
}

void recv_cb_s(const CommonAPI::CallStatus &callStatus, const CommonTypes::a1Struct &valStruct)
{
    std::cout << "Receive callback for structure: a1.s = " << valStruct.getS()
              << ", valStruct.a2.b = " << (valStruct.getA2().getB() ? "TRUE" : "FALSE")
              << ", valStruct.a2.d = " << valStruct.getA2().getD()
              << std::endl;
}

void ClientM1()
{
    CommonAPI::Runtime::setProperty("LogContext", "E03C");
    CommonAPI::Runtime::setProperty("LogApplication", "E03C");
    CommonAPI::Runtime::setProperty("LibraryBase", "CAPITest");

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "commonapi.examplesA.Methods";

    std::shared_ptr<CAPITestAProxy<>> myProxy = runtime->buildProxy<CAPITestAProxy>(domain, instance, "TestB");

    if (!myProxy)
    {
        std::cout << "CAPITestProxy create failed" << std::endl;
        exit(0);
    }

    while (!myProxy->isAvailable())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    if (CurrentModel == RunModel_All || CurrentModel == RunModel_Broadcast)
    {
        // Subscribe to broadcast
        myProxy->getMyStatusEvent().subscribe([&](const int32_t &val)
                                              { std::cout << "[ClientM1]Received boardcast event: " << val << std::endl; });
    }

    CommonAPI::CallStatus callStatus;

    int32_t value = 0;

    CommonAPI::CallInfo info(1000);
    info.sender_ = 5678;

    if (CurrentModel == RunModel_All || CurrentModel == RunModel_Getter)
    {
        // Get actual attribute value from service
        std::cout << "Getting attribute value: " << value << std::endl;
        myProxy->getXAttribute().getValue(callStatus, value, &info);
        if (callStatus != CommonAPI::CallStatus::SUCCESS)
        {
            std::cerr << "Remote call A failed!\n";
        }
        std::cout << "Got attribute value: " << value << std::endl;
    }

    if (CurrentModel == RunModel_All || CurrentModel == RunModel_Notifier)
    {
        // Subscribe for receiving values
        myProxy->getXAttribute().getChangedEvent().subscribe([&](const int32_t &val)
                                                             { std::cout << "Received change message: " << val << std::endl; });

        myProxy->getA1Attribute().getChangedEvent().subscribe([&](const CommonTypes::a1Struct &val)
                                                              { std::cout << "Received change message for A1" << std::endl; });
    }

    value = 100;

    if (CurrentModel == RunModel_All || CurrentModel == RunModel_Setter)
    {
        // Asynchronous call to set attribute of service
        std::cout << "Please input a integer:" << std::endl;
        std::cin >> value;
        std::cout << "Set data:" << value << std::endl;
        std::function<void(const CommonAPI::CallStatus &, int32_t)> fcb = recv_cb;
        myProxy->getXAttribute().setValueAsync(value, fcb);
    }

    // // Asynchronous call to set attribute of type structure in service
    // CommonTypes::a1Struct valueStruct;

    // valueStruct.setS("abc");
    // CommonTypes::a2Struct a2Struct = valueStruct.getA2();
    // a2Struct.setA(123);
    // a2Struct.setB(true);
    // a2Struct.setD(1234);
    // valueStruct.setA2(a2Struct);

    // std::function<void(const CommonAPI::CallStatus&, CommonTypes::a1Struct)> fcb_s = recv_cb_s;
    // myProxy->getA1Attribute().setValueAsync(valueStruct, fcb_s, &info);

    int32_t inX1 = 1;
    while (true)
    {
        if (CurrentModel == RunModel_All || CurrentModel == RunModel_Method)
        {
            std::string inX2 = "abc";
            CommonAPI::CallStatus callStatus;
            CAPITestA::stdErrorTypeEnum methodError;
            int32_t outY1;
            std::string outY2;

            // Synchronous call
            std::cout << "[ClientM1]Call foo with synchronous semantics ..." << std::endl;
            myProxy->foo(inX1, inX2, callStatus, methodError, outY1, outY2);
            std::cout << "[ClientM1]Input values: x1 = " << inX1
                      << ", x2 = " << inX2
                      << ", Output values: y1 = " << outY1
                      << ", y2 = " << outY2 << std::endl;

#if TEST_ASYNC
            // Asynchronous call
            std::cout << "Call foo with asynchronous semantics ..." << std::endl;

            std::function<
                void(const CommonAPI::CallStatus &,
                     const CAPITestA::stdErrorTypeEnum &,
                     const int32_t &,
                     const std::string &)>
                fcb = recv_cb;
            myProxy->fooAsync(inX1, inX2, recv_cb);
#endif
        }
        inX1++;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
