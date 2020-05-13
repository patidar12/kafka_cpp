#include <string>
#include <iostream>
#include <librdkafka/rdkafka.h>
using namespace std;
class Kafka{
private:
    rd_kafka_t *kafkaHandle;
    rd_kafka_topic_t *topicHandle;
    rd_kafka_type_t serviceType;
    string serviceName;
    string brokers = "localhost:9092";
    string topic = "test";
    int partition = 0;
    void fetchServiceType(string);
    void createConfiguration(void);
    void startConsumer(void);
    void stopConsumer(void);
public:
    Kafka(string);
    ~Kafka(void);
    bool produceMessage(string);
    string consumeMessage(void);
};
