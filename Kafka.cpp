#include "Kafka.h"


Kafka::Kafka(string serviceName){
    fetchQueueType(serviceName);	
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
    rd_kafka_topic_destroy(rkt);
    /* Destroy the handle */
    rd_kafka_destroy(rk);
    if (topic_conf)
        rd_kafka_topic_conf_destroy(topic_conf);
    /* Let background threads clean up and terminate cleanly. */
    int run = 5;
    while (run-- > 0 && rd_kafka_wait_destroyed(1000) == -1)
        cout << "Waiting for librdkafka to decommission\n";

}
void Kafka::fetchQueueType(string serviceName){
    serviceName = serviceName;	  
    if(serviceName == "producer"){   
        messageQueueType = RD_KAFKA_PRODUCER;
    }else{

        messageQueueType = RD_KAFKA_CONSUMER;
    }

}


void Kafka::startConsumer(){

    if (rd_kafka_consume_start(rkt, partition, 0) == -1){
        err = rd_kafka_last_error();
        cout << "Failed to start consuming/n";
    }

}
void Kafka::stopConsumer(){
      /* Stop consuming */
       rd_kafka_consume_stop(rkt, partition);

      while (rd_kafka_outq_len(rk) > 0)
           rd_kafka_poll(rk, 10);


}

void Kafka::createConfiguration(void){	
    /* Kafka configuration */
    conf = rd_kafka_conf_new();

    rd_kafka_conf_set(conf, "internal.termination.signal", tmp.c_str(), NULL, 0);

    /* Topic configuration */
    topic_conf = rd_kafka_topic_conf_new();

    rd_kafka_conf_set(conf, "enable.partition.eof", "true",NULL, 0);
    /* Create Kafka handle */
    if (!(rk = rd_kafka_new(messageQueueType, conf,errstr, sizeof(errstr)))) {
        cout << "%% Failed to create new producer: "<<errstr << endl;
        exit(1);
    }

    /* Add brokers */
    if (rd_kafka_brokers_add(rk, brokers.c_str()) == 0) {
        cout << "%% No valid brokers specified\n";
        exit(1);
    }

    /* Create topic */
    rkt = rd_kafka_topic_new(rk, topic.c_str(), topic_conf);
    topic_conf = NULL; /* Now owned by topic */
}
void Kafka::produceMessage(string message){

    int len = message.length();
    err = RD_KAFKA_RESP_ERR_NO_ERROR;

    /* Send/Produce message. */
    if (rd_kafka_produce(rkt, partition,RD_KAFKA_MSG_F_COPY,&message[0],len,NULL, 0,NULL) == -1) {
        err = rd_kafka_last_error();
    }
    if (err) {
        cout << "%% Failed to produce to topic : "<< rd_kafka_topic_name(rkt) << "partition : " <<  partition << " : " << rd_kafka_err2str(err) << endl;
        /* Poll to handle delivery reports */
        rd_kafka_poll(rk, 0);
	return;
    }
    /* Wait for messages to be delivered */
    while (rd_kafka_outq_len(rk) > 0)
        rd_kafka_poll(rk, 100);
    cout << "Sent message to Topic :" << rd_kafka_topic_name(rkt) << ", partition : " << partition << endl;
}

string Kafka::consumeMessage(void){


        rd_kafka_message_t *rkmessage;

       /* Consume single message.  */
       rkmessage = rd_kafka_consume(rkt, partition, 1000);
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
