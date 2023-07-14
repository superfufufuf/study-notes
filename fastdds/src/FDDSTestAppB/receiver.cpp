#include "receiver.h"

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::rtps;

#define LOG_HEAD "func[" + std::string(__func__) + "]"

Receiver::Receiver()
    : participant_(nullptr), subscriber_(nullptr), topic_(nullptr), reader_(nullptr), type_(new UsrDataPubSubType())
{
}

Receiver::~Receiver()
{
    if (reader_ != nullptr)
    {
        subscriber_->delete_datareader(reader_);
    }
    if (topic_ != nullptr)
    {
        participant_->delete_topic(topic_);
    }
    if (subscriber_ != nullptr)
    {
        participant_->delete_subscriber(subscriber_);
    }
    DomainParticipantFactory::get_instance()->delete_participant(participant_);
}

bool Receiver::init()
{
    // Create Participant
    DomainParticipantQos pqos;
    pqos.wire_protocol().builtin.discovery_config.leaseDuration = eprosima::fastrtps::c_TimeInfinite;
    pqos.name("Participant_sub");

    participant_ = DomainParticipantFactory::get_instance()->create_participant(0, pqos);
    if (participant_ == nullptr)
    {
        return false;
    }

    // Register Type
    type_.register_type(participant_);

    // Create Subscriber
    subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT);

    if (subscriber_ == nullptr)
    {
        return false;
    }

    // Create Topic
    topic_ = participant_->create_topic("UsrDataTopic", "UsrData", TOPIC_QOS_DEFAULT);

    if (topic_ == nullptr)
    {
        return false;
    }

    // Create DataReader
    DataReaderQos rqos;
    rqos.history().kind = KEEP_ALL_HISTORY_QOS;
    rqos.history().depth = 30;
    rqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    rqos.durability().kind = TRANSIENT_LOCAL_DURABILITY_QOS;

    reader_ = subscriber_->create_datareader(topic_, rqos, &listener);
    if (reader_ == nullptr)
    {
        return false;
    }

    return true;
}

void Receiver::run(uint32_t number, uint32_t sleep_ms)
{
    std::cout << LOG_HEAD << "Subscriber running until " << number << " samples have been received, n_samples:" << this->listener.n_samples << std::endl;
    while (number > this->listener.n_samples)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Now wait and try to remove from history
    std::cout << std::endl
              << LOG_HEAD << "Subscriber waiting for " << sleep_ms << " milliseconds" << std::endl
              << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));

    UsrDataPubSubType::type data;
    SampleInfo info;

    for (uint32_t i = 0; i < listener.n_samples; i++)
    {
        if (reader_->take_next_sample((void *)&data, &info) == ReturnCode_t::RETCODE_OK)
        {
            std::cout << LOG_HEAD << "Message " << data.key() << " " << data.value() << " read from history" << std::endl;
        }
        else
        {
            std::cout << LOG_HEAD << "Could not read message " << i << " from history" << std::endl;
        }
    }
}

void Receiver::SubListener::on_data_available(DataReader *reader)
{
    std::cout << LOG_HEAD << "entry" << std::endl;
    // SampleInfo info;
    // while (reader->read_next_sample(&hello, &info) == ReturnCode_t::RETCODE_OK)
    // {
    //     if (info.valid_data)
    //     {
            this->n_samples++;
    //         // Print your structure data here.
    //         std::cout << LOG_HEAD << "Message " << hello.key() << " " << hello.value() << " RECEIVED" << std::endl;
    //     }
    // }
}

void Receiver::SubListener::on_subscription_matched(DataReader *, const SubscriptionMatchedStatus &info)
{
    if (info.current_count_change == 1)
    {
        n_matched = info.total_count;
        std::cout << LOG_HEAD << "Subscriber matched." << std::endl;
    }
    else if (info.current_count_change == -1)
    {
        n_matched = info.total_count;
        std::cout << LOG_HEAD << "Subscriber unmatched." << std::endl;
    }
    else
    {
        std::cout << LOG_HEAD << info.current_count_change
                  << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
    }
}
