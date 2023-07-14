#include <thread>
#include <iostream>
#include <CommonAPI/CommonAPI.hpp>
#include "CAPITestBStubImpl.hpp"
#include "ModelB.h"

void ServerN()
{
    CommonAPI::Runtime::setProperty("LogContext", "E03S");
    CommonAPI::Runtime::setProperty("LogApplication", "E03S");
    CommonAPI::Runtime::setProperty("LibraryBase", "CAPITest");

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "commonapi.examplesB.Methods";

    std::shared_ptr<CAPITestBStubImpl> myService = std::make_shared<CAPITestBStubImpl>();
    while (!runtime->registerService(domain, instance, myService, "TestB"))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Successfully Registered Service!" << std::endl;

    while (true)
    {
        std::cout << "[ServerN]alive..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
