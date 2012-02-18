/*
HH10D humidity sensor sample code.
 (C) Semyon Tushev, 2011.
 License: CC-BY-SA
 
 http://tushev.org
 
 The circuit:
 * SCL to A5 via level-shifter (5V-3V3)
 * SDA to A4 via level-shifter (5V-3V3)
 * FOUT to digital pin 5
 * VDD to 3V3
 * GND to GND
 + pullup resistors for i2c(4k7, 10k) (http://www.nxp.com/news/backgrounders/bg_esc9727/)
 
 HH10D real I2C address is 81 (the datasheet contains error).
 
 */

#include <FreqCounter.h>
//#include <Wire.h>

int freq;
int offset=7753;
int sens=398;


void setup(){
//  Wire.begin();
  Serial.begin(115200);
  //  sens    =  i2cRead2bytes(81, 10); //Read sensitivity from EEPROM
  //  offset =  i2cRead2bytes(81, 12); //Same for offset

}
void loop(){
  //Get Frequency
  FreqCounter::f_comp= 8;             // Set compensation to 12
  FreqCounter::start(1000);            // Start counting with gatetime of 1000ms
  while (FreqCounter::f_ready == 0)         // wait until counter ready
    freq=FreqCounter::f_freq;            // read result

  //Calculate RH
  float RH =  (double(offset)-double(freq))*double(sens)/double(4096); //Sure, you can use int - depending on what do you need

  Serial.println(RH);

}
/*
int i2cRead2bytes(int deviceaddress, byte address) 
{
  // SET ADDRESS
  Wire.beginTransmission(deviceaddress);
  Wire.write(address); // address for sensitivity
  Wire.endTransmission();

  // REQUEST RETURN VALUE
  Wire.requestFrom(deviceaddress, 2);
  // COLLECT RETURN VALUE
  int rv = 0;
  for (int c = 0; c < 2; c++ )
    if (Wire.available()) rv = rv * 256 + Wire.read();
  return rv;
}
*/


