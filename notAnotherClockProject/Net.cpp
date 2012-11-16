#include "Net.h"

#include <stdlib.h>

#include <Ethernet.h>
#include <EthernetInterrupt.h>

Net::Net(){
  // Initialize Ethernet
  byte mac[]={
    0x90, 0xA2, 0xDA, 0x0D, 0x02, 0xD0    };
  IPAddress ip(192,168,1, 77);
  IPAddress dns(192,168,1, 1);
  IPAddress gateway(192,168,1, 1);
  IPAddress subnet(255,255,255,0);
  Ethernet.begin(mac, ip, dns, gateway, subnet);
  // Processors
  processor=NULL;
  processorCount=0;
  // Enable interrupts
  EthernetInterrupt::begin(0);
}

void Net::processAll(){  
  if(EthernetInterrupt::available()){
Serial.println("EtherInt");
    for(uint8_t c=0;c<processorCount;c++){
      processor[c]();
    }
    EthernetInterrupt::next();
  }
}

void Net::addProcessor(void (*l)()){
  processor=(void (**)())realloc(
    processor,
    sizeof(
        void (*)
    ) 
    * ++processorCount
  );
  processor[processorCount-1]=l;
}

Net::~Net(){
  free(processor);
}
