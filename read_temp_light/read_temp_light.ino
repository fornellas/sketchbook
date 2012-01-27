#include <Thermistor.h>

#define APIN_PHOTORESISTOR 1
#define APIN_THERMISTOR 0

void printDouble(double val, byte precision) {
  // prints val with number of decimal places determine by precision
  // precision is a number from 0 to 6 indicating the desired decimal places
  // example: printDouble(3.1415, 2); // prints 3.14 (two decimal places)
  Serial.print (int(val));  //prints the int part
  if( precision > 0) {
    Serial.print("."); // print the decimal point
    unsigned long frac, mult = 1;
    byte padding = precision -1;
    while(precision--) mult *=10;
    if(val >= 0) frac = (val - int(val)) * mult; 
    else frac = (int(val) - val) * mult;
    unsigned long frac1 = frac;
    while(frac1 /= 10) padding--;
    while(padding--) Serial.print("0");
    Serial.print(frac,DEC) ;
  }
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  Thermistor temp(APIN_THERMISTOR);
  while(1){
    Serial.print("Light: ");
    Serial.print(analogRead(APIN_PHOTORESISTOR));
    Serial.print(" Temp: ");
    printDouble(temp.read(), 3);
    Serial.println("C");

    delay(200);
  }
}



