#include <DS18S20.h>
#include <OneWire.h>

double DS18S20::readK(){
  uint8_t data[2];

  // CONVERT
  ow->reset();
  ow->select(addr);
  ow->write(0x44); 

  // Read Scratchpad
  ow->reset();
  ow->select(addr);    
  ow->write(0xBE); 
  for (int i = 0; i < 2; i++)
    data[i] = ow->read();
  
  // Calculate
  return convC2K((double)((data[1]<<8)|data[0])/16.0);
}

DS18S20::DS18S20(uint8_t *a, OneWire *o){
  addr=a;
  ow=o;
}
