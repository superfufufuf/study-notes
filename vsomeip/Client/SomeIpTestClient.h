#ifndef SomeIpTestClient_H
#define SomeIpTestClient_H

#include <thread>
#include <vsomeip/vsomeip.hpp>

class SomeIpTestClient
{
public:
    SomeIpTestClient() = delete;
    SomeIpTestClient(const vsomeip::service_t &_serviceId, const vsomeip::instance_t &_instanceId, const vsomeip::method_t &_methodId);
    SomeIpTestClient(const SomeIpTestClient &_other) = delete;
    SomeIpTestClient(const SomeIpTestClient &&_other) = delete;
    ~SomeIpTestClient();

    bool init();
    void start();
    void stop();
    bool SendMsg(const std::string &data);
private:

    void on_state_cbk(vsomeip::state_type_e _state);
    void on_availability_cbk(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available);
    void on_message_cbk(const std::shared_ptr<vsomeip::message> &_response);

    std::shared_ptr<vsomeip::runtime> m_runTime;
    std::shared_ptr<vsomeip::application> m_app;
    vsomeip::service_t m_serviceId;
    vsomeip::instance_t m_instanceId;
    vsomeip::method_t m_methodId;
    bool bAvailable = false;
};

#endif