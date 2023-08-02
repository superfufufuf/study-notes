#ifndef SomeIpTestServer_H
#define SomeIpTestServer_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vsomeip/vsomeip.hpp>

class SomeIpTestServer
{
public:
    SomeIpTestServer();
    SomeIpTestServer(const SomeIpTestServer &_other) = delete;
    SomeIpTestServer(const SomeIpTestServer &&_other) = delete;
    ~SomeIpTestServer();

    bool init();
    void start();
    void stop();
    void terminate();
    void testCommonapi();
    void PublishData(const std::string &data);

private:
    void on_state_cbk(vsomeip::state_type_e _state);
    void on_message_cbk(const std::shared_ptr<vsomeip::message> &_response);

    std::shared_ptr<vsomeip::runtime> m_runTime;
    std::shared_ptr<vsomeip::application> m_app;

    bool bStop;
    std::mutex m_mtx;
    std::condition_variable m_condition;
    std::thread m_stop_thread;
};

#endif