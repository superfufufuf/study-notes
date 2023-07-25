#ifndef SomeIpTestServer_H
#define SomeIpTestServer_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vsomeip/vsomeip.hpp>

class SomeIpTestServer
{
public:
    SomeIpTestServer() = delete;
    SomeIpTestServer(const vsomeip::service_t &_serviceId, const vsomeip::instance_t &_instanceId, const vsomeip::method_t &_methodId);
    SomeIpTestServer(const SomeIpTestServer &_other) = delete;
    SomeIpTestServer(const SomeIpTestServer &&_other) = delete;
    ~SomeIpTestServer();

    bool init();
    void start();
    void stop();
    void terminate();
    void on_state_cbk(vsomeip::state_type_e _state);
    void on_message_cbk(const std::shared_ptr<vsomeip::message> &_response);

private:
    std::shared_ptr<vsomeip::runtime> m_runTime;
    std::shared_ptr<vsomeip::application> m_app;
    vsomeip::service_t m_serviceId;
    vsomeip::instance_t m_instanceId;
    vsomeip::method_t m_methodId;

    bool bStop;
    std::mutex m_mtx;
    std::condition_variable m_condition;
    std::thread m_stop_thread;
};

#endif