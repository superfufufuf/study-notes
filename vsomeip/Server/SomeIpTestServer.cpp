#include <thread>
#include <iostream>
#include <functional>
#include <cstdio>
#include "SomeIpTestServer.h"

using namespace std;

SomeIpTestServer::SomeIpTestServer(const vsomeip::service_t &_serviceId, const vsomeip::instance_t &_instanceId, const vsomeip::method_t &_methodId)
    : m_runTime(vsomeip::runtime::get()),
      m_app(m_runTime->create_application("TestA")),
      m_serviceId(_serviceId),
      m_instanceId(_instanceId),
      m_methodId(_methodId),
      bStop(false),
      m_stop_thread(std::bind(&SomeIpTestServer::stop, this))
{
}

SomeIpTestServer::~SomeIpTestServer()
{
    terminate();
    m_stop_thread.join();
}

bool SomeIpTestServer::init()
{
    // init the application
    if (!m_app->init())
    {
        cout << "Couldn't initialize application" << endl;
        return false;
    }

    // register a message handler callback for messages sent to our service
    m_app->register_message_handler(m_serviceId, m_instanceId,
                                    m_methodId,
                                    std::bind(&SomeIpTestServer::on_message_cbk, this,
                                              std::placeholders::_1));

    // register a state handler to get called back after registration at the
    // runtime was successful
    m_app->register_state_handler(
        std::bind(&SomeIpTestServer::on_state_cbk, this,
                  std::placeholders::_1));
    return true;
}

void SomeIpTestServer::start()
{
    // start the application and wait for the on_event callback to be called
    // this method only returns when m_app->stop() is called
    m_app->start();
}

void SomeIpTestServer::stop()
{
    std::unique_lock<std::mutex> its_lock(m_mtx);
    while (!bStop)
    {
        m_condition.wait(its_lock);
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
    // Stop offering the service
    m_app->stop_offer_service(m_serviceId, m_instanceId);
    // unregister the state handler
    m_app->unregister_state_handler();
    // unregister the message handler
    m_app->unregister_message_handler(m_serviceId, m_instanceId,
                                      m_methodId);
    // shutdown the application
    m_app->stop();
}

void SomeIpTestServer::terminate()
{
    std::lock_guard<std::mutex> its_lock(m_mtx);
    bStop = true;
    m_condition.notify_one();
}

void SomeIpTestServer::on_state_cbk(vsomeip::state_type_e _state)
{
    if (_state == vsomeip::state_type_e::ST_REGISTERED)
    {
        // we are registered at the runtime and can offer our service
        m_app->offer_service(m_serviceId, m_instanceId);
    }
}

void SomeIpTestServer::on_message_cbk(const std::shared_ptr<vsomeip::message> &_request)
{
    // Create a response based upon the request
    std::shared_ptr<vsomeip::message> resp = m_runTime->create_response(_request);

    // Construct string to send back
    std::string str("Hello ");
    std::string recvData;
    recvData.append(reinterpret_cast<const char *>(_request->get_payload()->get_data()),
                    0, _request->get_payload()->get_length());
    str.append(recvData);
    cout << "recv data: " << recvData << ", send data: " << str << endl;

    // Create a payload which will be sent back to the client
    std::shared_ptr<vsomeip::payload> resp_pl = m_runTime->create_payload();
    std::vector<vsomeip::byte_t> pl_data(str.begin(), str.end());
    resp_pl->set_data(pl_data);
    resp->set_payload(resp_pl);

    // Send the response back
    m_app->send(resp);
    // we have finished
}