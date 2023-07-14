#include <thread>
#include <iostream>
#include <unistd.h>
#include <CommonAPI/CommonAPI.hpp>
#include "ModelA.h"
#include <v1/commonapi/examplesA/CAPITestAProxy.hpp>

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

    // Subscribe to broadcast
    myProxy->getMyStatusEvent().subscribe([&](const int32_t &val)
                                          { std::cout << "[ClientM1]Received boardcast event: " << val << std::endl; });

    int32_t inX1 = 1;
    while (true)
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
        inX1++;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
