#include <iostream>
#include <fstream>
#include "Kafka.cpp"
using namespace std;
int main(){
   ofstream logFile;	
   Kafka *obj = new Kafka("consumer");
   logFile.open("message.txt",ios::app);
   for(int i=0;i<150;i++){
      string message = obj->consumeMessage();
      if(message == "NULL") break;
      logFile << message << endl;
      
   }
   logFile.close();
   delete obj;

return 0;

}
