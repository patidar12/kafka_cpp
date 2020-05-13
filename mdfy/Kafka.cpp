#include "Kafka.h"


Kafka::Kafka(string serviceName){
    fetchServiceType(serviceName);
    createConfiguration();
    if(serviceName == "consumer"){
	  startConsumer();
    }
}
Kafka::~Kafka(){

    if(serviceName == "consumer"){
        stopConsumer();
    }

    /* Destroy topic */
    rd_kafka_topic_destroy(topicHandle);
    /* Destroy the handle */
    rd_kafka_destroy(kafkaHandle);
    /* Let background threads clean up and terminate cleanly. */
    int run = 5;
    while (run-- > 0 && rd_kafka_wait_destroyed(1000) == -1)
        cout << "Waiting for librdkafka to decommission\n";

}
void Kafka::fetchServiceType(string serviceName){
    this->serviceName = serviceName;	  
    if(serviceName == "producer"){   
        serviceType = RD_KAFKA_PRODUCER;
    }else if(serviceName == "consumer"){
        serviceType = RD_KAFKA_CONSUMER;
    }else{
        cout << "servicename are not correct!"<< endl;
	exit(1);
    }
}


void Kafka::startConsumer(){

    rd_kafka_resp_err_t err;	
    if (rd_kafka_consume_start(topicHandle, partition, 0) == -1){
        err = rd_kafka_last_error();
        cout << "Failed to start consuming "<< err;
	exit(1);
    }

}
void Kafka::stopConsumer(){
      /* Stop consuming */
       if(rd_kafka_consume_stop(topicHandle, partition) == -1){
           cout << "Filed to stop consumer!"<<endl;
       }

      while (rd_kafka_outq_len(kafkaHandle) > 0)
           rd_kafka_poll(kafkaHandle, 10);

}

void Kafka::createConfiguration(void){
    
    rd_kafka_conf_t *conf;
    rd_kafka_topic_conf_t *topic_conf;
    string tmp = "600";
    char errstr[512];

    /* Kafka configuration */
    conf = rd_kafka_conf_new();

    rd_kafka_conf_set(conf, "internal.termination.signal", tmp.c_str(), NULL, 0);

    /* Topic configuration */
    topic_conf = rd_kafka_topic_conf_new();

    rd_kafka_conf_set(conf, "enable.partition.eof", "true",NULL, 0);
    /* Create Kafka handle */
    if (!(kafkaHandle = rd_kafka_new(serviceType, conf,errstr, sizeof(errstr)))) {
        cout << "%% Failed to create new "<< serviceName <<" "<<errstr << endl;
        exit(1);
    }

    /* Add brokers */
    if (rd_kafka_brokers_add(kafkaHandle, brokers.c_str()) == 0) {
        cout << "%% No valid brokers specified\n";
        exit(1);
    }

    /* Create topic */
    topicHandle = rd_kafka_topic_new(kafkaHandle, topic.c_str(), topic_conf);
    topic_conf = NULL; /* Now owned by topic */
}
bool Kafka::produceMessage(string message){

    rd_kafka_resp_err_t err;	
    int len = message.length();
    err = RD_KAFKA_RESP_ERR_NO_ERROR;

    /* Send/Produce message. */
    if (rd_kafka_produce(topicHandle, partition,RD_KAFKA_MSG_F_COPY,&message[0],len,NULL, 0,NULL) == -1) {
        err = rd_kafka_last_error();
    }
    if (err) {
        cout << "%% Failed to produce to topic : "<< rd_kafka_topic_name(topicHandle) << "partition : " <<  partition << " : " << rd_kafka_err2str(err) << endl;
        /* Poll to handle delivery reports */
        rd_kafka_poll(kafkaHandle, 0);
        return false;	
    }
    /* Wait for messages to be delivered */
    while (rd_kafka_outq_len(kafkaHandle) > 0)
        rd_kafka_poll(kafkaHandle, 100);
    cout << "Sent message to Topic :" << rd_kafka_topic_name(topicHandle) << ", partition : " << partition << endl;
    return true;
}

string Kafka::consumeMessage(void){


        rd_kafka_message_t *rkmessage;

       /* Consume single message.  */
       rkmessage = rd_kafka_consume(topicHandle, partition, 1000);
       if(!rkmessage){
	       cout << "Messege Consuming TimeOut!"<< endl;
	       rd_kafka_message_destroy(rkmessage);
	       return "NULL";
       }

       if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF){
	       cout << "Reached at the end!" << endl;
	       rd_kafka_message_destroy(rkmessage);
	       return "NULL";
       }

      string message((const char *) (rkmessage->payload));
      /* Return message to rdkafka */
      rd_kafka_message_destroy(rkmessage);

       return message;

   }
