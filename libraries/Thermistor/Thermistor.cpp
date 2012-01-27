/*
  Thermistor.cpp - Temperature reading library for Arduino
  Based on the code found at:
  http://www.arduino.cc/playground/ComponentLib/Thermistor2

  Schematic:

  [Ground] ---- [10k Resistor] -------|------- [10k Thermistor] ---- [+5v]
                                      |
                                  Analog Pin
*/

extern "C" {
  #include <inttypes.h>
  #include <math.h>
}

#include <Thermistor.h>
#include <Arduino.h>

Thermistor::Thermistor(uint8_t pin) {
  this->pin=pin;
};

double Thermistor::read() {
  int RawADC;
  long Resistance;
  double Temp;

  RawADC=analogRead(pin);

 // Inputs ADC Value from Thermistor and outputs Temperature in Celsius
 //  requires: include <math.h>
 // Utilizes the Steinhart-Hart Thermistor Equation:
 //    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]^3}
 //    where A = 0.001129148, B = 0.000234125 and C = 8.76741E-08
 Resistance=((10240000/RawADC) - 10000);  // Assuming a 10k Thermistor.  Calculation is actually: Resistance = (1024/ADC)
 Temp = log(Resistance); // Saving the Log(resistance) so not to calculate it 4 times later. // "Temp" means "Temporary" on this line.
 Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));   // Now it means both "Temporary" and "Temperature"
 Temp = Temp - 273.15;  // Convert Kelvin to Celsius                                         // Now it only means "Temperature"

 // Uncomment this line for the function to return Fahrenheit instead.
 //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert to Fahrenheit
 return Temp;  // Return the Temperature
}
