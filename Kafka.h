#include <string>
#include <iostream>
#include <librdkafka/rdkafka.h>
using namespace std;
class Kafka{
private:
    rd_kafka_t *rk;
    rd_kafka_topic_t *rkt;
    rd_kafka_type_t serviceType;
    string brokers = "localhost:9092";
    string topic = "test";
    string serviceName;
    int partition = 0;
    char errstr[512];
    string tmp = "600";
    rd_kafka_resp_err_t err;
    rd_kafka_conf_t *conf;
    rd_kafka_topic_conf_t *topic_conf;

public:
    Kafka(string);
    ~Kafka(void);
    void startConsumer(void);
    void stopConsumer(void);
    void createConfiguration(void);
    //void createConfigurationForConsumer(void);
    void produceMessage(string);
    string consumeMessage(void);
    void fetchQueueType(string);
};
