#include <Thermistor.h>

void setup(){
  Serial.begin(115200);

}

#define AVG 50
byte p=0;
void loop(){
  float v[AVG];
  float t;
  float r;

  r=Thermistor::read(A2);
  v[p++]=r;
  if(p==AVG)
    p=0;

  t=0;
  for(byte i=0;i<AVG;i++)
    t+=v[i];
  t=t/AVG;

  Serial.print("Outdoor");
  Serial.print(t);
  Serial.print(" C (");
  Serial.print(r);
  Serial.println(" C)");
  delay(50);
}




