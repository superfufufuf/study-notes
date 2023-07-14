#ifndef _FDDS_TEST_RECEIVER_H_
#define _FDDS_TEST_RECEIVER_H_

#include "UsrDataPubSubTypes.h"
#include "UsrData.h"

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/topic/Topic.hpp>

class Receiver
{
public:
    Receiver();

    virtual ~Receiver();

    bool init();

    void run(uint32_t number, uint32_t sleep_ms);

private:
    eprosima::fastdds::dds::DomainParticipant *participant_;

    eprosima::fastdds::dds::Subscriber *subscriber_;

    eprosima::fastdds::dds::Topic *topic_;

    eprosima::fastdds::dds::DataReader *reader_;

    eprosima::fastdds::dds::TypeSupport type_;

public:
    class SubListener : public eprosima::fastdds::dds::DataReaderListener
    {
    public:
        SubListener()
            : n_matched(0), n_samples(0)
        {
        }

        ~SubListener() override
        {
        }

        void on_data_available(eprosima::fastdds::dds::DataReader *reader) override;

        void on_subscription_matched(eprosima::fastdds::dds::DataReader *reader, const eprosima::fastdds::dds::SubscriptionMatchedStatus &info) override;

        UsrData hello;

        int n_matched;

        uint32_t n_samples;

    } listener;
};

#endif