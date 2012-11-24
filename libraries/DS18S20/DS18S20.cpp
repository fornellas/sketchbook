#include <DS18S20.h>
#include <OneWire.h>

void DS18S20::loadFromSensor(){
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
  K=((double)((data[1]<<8)|data[0])/16.0)+273.15;
}

DS18S20::DS18S20(uint8_t *a, OneWire *o){
  addr=a;
  ow=o;
  // discart first reading, always wrong on power on
  loadFromSensor();
}
