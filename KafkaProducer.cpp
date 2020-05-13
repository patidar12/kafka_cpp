#include <iostream>
#include "Kafka.cpp"
using namespace std;
int main(void){
   Kafka *obj = new Kafka("producer");
   for(int i=0;i<3;i++)
      obj->produceMessage("Producer: Ï am Back!");
   delete obj;
}
