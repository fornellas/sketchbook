#include "EthernetInterrupt.h"

#include <Arduino.h>
#include <SPI.h>

void EthernetInterrupt::w5100IntProc(){
  w5100int=1;
  /* It seems that the W5100 will not keep /INT low when there is a new
     interrupt, as stated at the datasheet. /INT oscilates, and generates a LOT
     of unnecessary interrupts, leaving virtually no CPU time for other tasks.
     This is a workarround for that bougus behaviour.
  */
  detachInterrupt(interrupt);
}

uint8_t EthernetInterrupt::read(uint16_t _addr){
  setSS();
  SPI.transfer(0x0F);
  SPI.transfer(_addr >> 8);
  SPI.transfer(_addr & 0xFF);
  uint8_t _data = SPI.transfer(0);
  resetSS();
  return _data;
}

uint8_t EthernetInterrupt::write(uint16_t _addr, uint8_t _data){
  setSS();
  SPI.transfer(0xF0);
  SPI.transfer(_addr >> 8);
  SPI.transfer(_addr & 0xFF);
  SPI.transfer(_data);
  resetSS();
  return 1;
}

void EthernetInterrupt::begin(uint8_t i){
  w5100int=0;
  interrupt=i;
  next();
  writeIMR(0xEF);
}

uint8_t EthernetInterrupt::available(){
  if(w5100int){
    w5100int=0;
    writeS0_IR(readS0_IR());
    writeS1_IR(readS1_IR());
    writeS2_IR(readS2_IR());
    writeS3_IR(readS3_IR());
    return 1;
  }else
    return 0;
}

void EthernetInterrupt::next(){
  attachInterrupt(interrupt, w5100IntProc, LOW);
}
