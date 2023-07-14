#include <thread>
#include <iostream>
#include <unistd.h>
#include <CommonAPI/CommonAPI.hpp>
#include "ModelB.h"
#include <v1/commonapi/examplesB/CAPITestBProxy.hpp>

#define TEST_ASYNC 0

using namespace v1_2::commonapi::examplesB;

void ClientN1()
{
    CommonAPI::Runtime::setProperty("LogContext", "E03C");
    CommonAPI::Runtime::setProperty("LogApplication", "E03C");
    CommonAPI::Runtime::setProperty("LibraryBase", "CAPITest");

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "commonapi.examplesB.Methods";

    std::shared_ptr<CAPITestBProxy<>> myProxy = runtime->buildProxy<CAPITestBProxy>(domain, instance, "TestA");

    if (!myProxy)
    {
        std::cout << "CAPITestProxy create failed" << std::endl;
        exit(0);
    }

    while (!myProxy->isAvailable())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    int32_t inX1 = 1;
    while (true)
    {
        CommonAPI::CallStatus callStatus;
        CAPITestB::stdErrorTypeEnum methodError;
        int32_t outY1;

        // Synchronous call
        std::cout << "[ClientN1]Call foo with synchronous semantics ..." << std::endl;
        myProxy->foo(inX1, callStatus, methodError, outY1);
        std::cout << "[ClientN1]Input values: x1 = " << inX1 
        << ",  Output values: y1 = " << outY1 << std::endl;

        inX1++;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void ClientN2()
{
    CommonAPI::Runtime::setProperty("LogContext", "E03C");
    CommonAPI::Runtime::setProperty("LogApplication", "E03C");
    CommonAPI::Runtime::setProperty("LibraryBase", "CAPITest");

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "commonapi.examplesB.Methods";

    std::shared_ptr<CAPITestBProxy<>> myProxy = runtime->buildProxy<CAPITestBProxy>(domain, instance, "TestA");

    if (!myProxy)
    {
        std::cout << "CAPITestProxy create failed" << std::endl;
        exit(0);
    }

    while (!myProxy->isAvailable())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    int32_t inX1 = 100;
    while (true)
    {
        CommonAPI::CallStatus callStatus;
        CAPITestB::stdErrorTypeEnum methodError;
        int32_t outY1;

        // Synchronous call
        std::cout << "[ClientN2]Call foo with synchronous semantics ..." << std::endl;
        myProxy->foo(inX1, callStatus, methodError, outY1);
        std::cout << "[ClientN2]Input values: x1 = " << inX1 
        << ",  Output values: y1 = " << outY1 << std::endl;

        inX1++;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
