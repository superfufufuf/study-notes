#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <thread>
#include <chrono>
#include "sender.h"

using namespace std;
using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::rtps;

Sender::Sender()
    : participant_(nullptr), publisher_(nullptr), topic_(nullptr), writer_(nullptr), type_(new UsrDataPubSubType())
{
}

Sender::~Sender()
{
    if (writer_ != nullptr)
    {
        publisher_->delete_datawriter(writer_);
    }
    if (publisher_ != nullptr)
    {
        participant_->delete_publisher(publisher_);
    }
    if (topic_ != nullptr)
    {
        participant_->delete_topic(topic_);
    }
    DomainParticipantFactory::get_instance()->delete_participant(participant_);
}

bool Sender::init()
{
    m_data.key("Key");
    m_data.value("Value");

    // Create Participant
    DomainParticipantQos pqos;
    pqos.wire_protocol().builtin.discovery_config.leaseDuration = eprosima::fastrtps::c_TimeInfinite;
    pqos.name("Participant_pub");

    participant_ = DomainParticipantFactory::get_instance()->create_participant(0, pqos);
    if (participant_ == nullptr)
    {
        return false;
    }

    // Register Type
    type_.register_type(participant_);

    // Create Publisher
    publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT);

    if (publisher_ == nullptr)
    {
        return false;
    }

    // Create Topic
    topic_ = participant_->create_topic("UsrDataTopic", "UsrData", TOPIC_QOS_DEFAULT);

    if (topic_ == nullptr)
    {
        return false;
    }

    // Create DataWriter
    DataWriterQos wqos;
    wqos.history().kind = KEEP_ALL_HISTORY_QOS;
    wqos.history().depth = 30;
    wqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    wqos.durability().kind = TRANSIENT_LOCAL_DURABILITY_QOS;

    writer_ = publisher_->create_datawriter(topic_, wqos, &listener);
    if (writer_ == nullptr)
    {
        return false;
    }

    return true;
}

bool Sender::publish(bool waitForListener)
{
    if (listener.first_connected || !waitForListener || listener.n_matched > 0)
    {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        auto curMecs = chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
        m_data.key("cur time:");
        m_data.value(std::to_string(curMecs));
        writer_->write((void *)&m_data);
        return true;
    }
    return false;
}

void Sender::run(uint32_t samples)
{
    std::cout << "Publisher running" << std::endl;

    samples = (samples == 0) ? 10 : samples;
    for (uint32_t i = 0; i < samples; ++i)
    {
        if (!publish())
        {
            --i;
        }
        else
        {
            std::cout << "Message with index: " << m_data.value() << " SENT" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    size_t removed = 0;
    writer_->clear_history(&removed);
}

void Sender::runThread(uint32_t number, uint32_t sleep)
{
}

void Sender::PubListener::on_publication_matched(eprosima::fastdds::dds::DataWriter *writer, const eprosima::fastdds::dds::PublicationMatchedStatus &info)
{
    if (info.current_count_change == 1)
    {
        n_matched = info.total_count;
        first_connected = true;
        std::cout << "Publisher matched." << std::endl;
    }
    else if (info.current_count_change == -1)
    {
        n_matched = info.total_count;
        std::cout << "Publisher unmatched." << std::endl;
    }
    else
    {
        std::cout << info.current_count_change
                  << " is not a valid value for PublicationMatchedStatus current count change" << std::endl;
    }
}
