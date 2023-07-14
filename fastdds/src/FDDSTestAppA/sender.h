#ifndef _FDDS_TEST_SENDER_H_
#define _FDDS_TEST_SENDER_H_

#include "UsrDataPubSubTypes.h"

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/topic/Topic.hpp>

class Sender
{
public:
    Sender();
    ~Sender();

    bool init();

    bool publish(bool waitForListener = true);

    void run(uint32_t samples);

private:
    UsrData m_data;

    eprosima::fastdds::dds::DomainParticipant *participant_;

    eprosima::fastdds::dds::Publisher *publisher_;

    eprosima::fastdds::dds::Topic *topic_;

    eprosima::fastdds::dds::DataWriter *writer_;

    bool stop;

    class PubListener : public eprosima::fastdds::dds::DataWriterListener
    {
    public:
        PubListener()
            : n_matched(0), first_connected(false)
        {
        }

        ~PubListener() override
        {
        }

        void on_publication_matched(eprosima::fastdds::dds::DataWriter *writer, const eprosima::fastdds::dds::PublicationMatchedStatus &info) override;

        int n_matched;

        bool first_connected;

    } listener;

    void runThread(uint32_t number, uint32_t sleep);

    eprosima::fastdds::dds::TypeSupport type_;
};

#endif