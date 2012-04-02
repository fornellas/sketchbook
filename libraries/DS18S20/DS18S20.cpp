#include <DS18S20.h>
#include <OneWire.h>

float DS18S20::read(uint8_t pin, uint8_t *addr){
  uint8_t data[2];

  // start OneWire
  OneWire ow(pin);

  // CONVERT
  ow.reset();
  ow.select(addr);
  ow.write(0x44); 

  // Read Scratchpad
  ow.reset();
  ow.select(addr);    
  ow.write(0xBE); 
  for (int i = 0; i < 2; i++)
    data[i] = ow.read();
  
  // Calculate
  return (float)((data[1]<<8)|data[0])/16.0;
}
