#include <thread>
#include <iostream>
#include <CommonAPI/CommonAPI.hpp>
#include "CAPITestAStubImpl.hpp"
#include "ModelA.h"

void ServerM()
{
    CommonAPI::Runtime::setProperty("LogContext", "E03S");
    CommonAPI::Runtime::setProperty("LogApplication", "E03S");
    CommonAPI::Runtime::setProperty("LibraryBase", "CAPITest");

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "commonapi.examplesA.Methods";

    std::shared_ptr<CAPITestAStubImpl> myService = std::make_shared<CAPITestAStubImpl>();
    while (!runtime->registerService(domain, instance, myService, "TestA"))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Successfully Registered Service!" << std::endl;

    while (true)
    {
        myService->incCounter(); // Change value of attribute, see stub implementation
        std::cout << "[ServerM]alive..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
