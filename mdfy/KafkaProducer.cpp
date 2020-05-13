#include <iostream>
#include "Kafka.cpp"
using namespace std;
int main(void){
   Kafka *obj = new Kafka("producer");
   for(int i=0;i<3;i++){
      bool status = obj->produceMessage("Producer: Ï am Back!");
      if(!status){
          cout << "Failed to produce message";
      }
   }
   delete obj;
}
