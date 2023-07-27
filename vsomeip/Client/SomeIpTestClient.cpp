#include <thread>
#include <iostream>
#include <functional>
#include <cstdio>
#include "SomeIpTestClient.h"
#include "SomeIpTestConst.h"

using namespace std;

SomeIpTestClient::SomeIpTestClient()
    : m_runTime(vsomeip::runtime::get()),
      m_app(m_runTime->create_application("TestB"))
{
}

SomeIpTestClient::~SomeIpTestClient()
{
    stop();
}

bool SomeIpTestClient::init()
{
    // init the application
    if (!m_app->init())
    {
        cout << "Couldn't initialize application" << endl;
        return false;
    }

    // register a state handler to get called back after registration at the
    // runtime was successful
    m_app->register_state_handler(
        std::bind(&SomeIpTestClient::on_state_cbk, this,
                  std::placeholders::_1));

    // register a callback for responses from the service
    m_app->register_message_handler(vsomeip::ANY_SERVICE,
                                    instanceIdA, vsomeip::ANY_METHOD,
                                    std::bind(&SomeIpTestClient::on_message_cbk, this,
                                              std::placeholders::_1));

    // register a callback which is called as soon as the service is available
    m_app->register_availability_handler(serviceIdA, instanceIdA,
                                         std::bind(&SomeIpTestClient::on_availability_cbk, this,
                                                   std::placeholders::_1, std::placeholders::_2,
                                                   std::placeholders::_3));

    std::set<vsomeip::eventgroup_t> its_groups;
    its_groups.insert(eventGroupA);
    m_app->request_event(serviceIdA, instanceIdA, eventA, its_groups, vsomeip_v3::event_type_e::ET_EVENT);
    m_app->subscribe(serviceIdA, instanceIdA, eventGroupA);
    return true;
}

void SomeIpTestClient::start()
{
    // start the application and wait for the on_event callback to be called
    // this method only returns when m_app->stop() is called
    m_app->start();
}

void SomeIpTestClient::stop()
{
    // unregister the state handler
    m_app->unregister_state_handler();
    // unregister the message handler
    m_app->unregister_message_handler(vsomeip::ANY_SERVICE, instanceIdA, vsomeip::ANY_METHOD);
    // alternatively unregister all registered handlers at once
    m_app->clear_all_handler();
    // release the service
    m_app->release_service(serviceIdA, instanceIdA);
    // shutdown the application
    m_app->stop();
}

bool SomeIpTestClient::SendMsg(const std::string &data)
{
    if (bAvailable)
    {
        // The service is available then we send the request
        // Create a new request
        std::shared_ptr<vsomeip::message> rq = m_runTime->create_request();
        // Set the hello world service as target of the request
        rq->set_service(serviceIdA);
        rq->set_instance(instanceIdA);
        rq->set_method(methodIdA);

        // Create a payload which will be sent to the service
        std::shared_ptr<vsomeip::payload> pl = m_runTime->create_payload();
        std::vector<vsomeip::byte_t> pl_data(std::begin(data), std::end(data));

        pl->set_data(pl_data);
        rq->set_payload(pl);
        // Send the request to the service. Response will be delivered to the
        // registered message handler
        cout << "Sending: " << data << endl;
        m_app->send(rq);
        return true;
    }
    return false;
}

void SomeIpTestClient::on_state_cbk(vsomeip::state_type_e _state)
{
    cout << "on_state_cbk, _state:" << static_cast<int>(_state) << endl;
    if (_state == vsomeip::state_type_e::ST_REGISTERED)
    {
        // we are registered at the runtime now we can request the service
        // and wait for the on_availability callback to be called
        m_app->request_service(serviceIdA, instanceIdA);
    }
}

void SomeIpTestClient::on_availability_cbk(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available)
{
    cout << "on_availability_cbk, _service:" << _service << ", _instance: " << _instance << ", _is_available: " << _is_available << endl;
    // Check if the available service is the the hello world service
    if (serviceIdA == _service && instanceIdA == _instance)
    {
        bAvailable = _is_available;
    }
}

void SomeIpTestClient::on_message_cbk(const std::shared_ptr<vsomeip::message> &_response)
{
    cout << "on_message_cbk, _service:" << _response->get_service()
         << ", _instance: " << _response->get_instance()
         << ", message_type: " << static_cast<int>(_response->get_message_type())
         << ", return_code: " << static_cast<int>(_response->get_return_code()) << endl;

    if (serviceIdA == _response->get_service() &&
        instanceIdA == _response->get_instance() &&
        // vsomeip::message_type_e::MT_RESPONSE == _response->get_message_type() &&
        vsomeip::return_code_e::E_OK == _response->get_return_code())
    {
        // Get the payload and print it
        std::shared_ptr<vsomeip::payload> pl = _response->get_payload();
        std::string resp = std::string(reinterpret_cast<const char *>(pl->get_data()), 0, pl->get_length());
        cout << "Received[" << static_cast<int>(_response->get_message_type()) << "]: " << resp << endl;
    }
}
