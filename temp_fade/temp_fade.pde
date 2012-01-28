#define ThermistorPIN 0
#define PIN_RED 9
#define PIN_GREEN 10
#define PIN_BLUE 11
#define LIGHT 25
#define SCALE_MIN 23.0
#define SCALE_MAX 28.0

static int colors[] = {
  0,     0,     0,
  0,     0,     LIGHT,
  0,     LIGHT, 0,
  0,     LIGHT, LIGHT,
  LIGHT, 0,     0,
  LIGHT, 0,     LIGHT,
  LIGHT, LIGHT, 0,
  LIGHT, LIGHT, LIGHT
};

#define C_BLACK    0
#define C_BLUE     3
#define C_GREEN    6
#define C_CYAN     9
#define C_RED     12
#define C_MAGENTA 15
#define C_YELLOW  18
#define C_WHITE   21

static int scale[] = {
  C_BLUE,
  C_GREEN,
  C_RED,
};
#define SCALE_SIZE 3

#include <math.h>
//Schematic:
// [Ground] ---- [10k-Resister] -------|------- [Thermistor] ---- [+5v]
//                                     |
//                                Analog Pin 0

double Thermistor(int RawADC) {
 // Inputs ADC Value from Thermistor and outputs Temperature in Celsius
 //  requires: include <math.h>
 // Utilizes the Steinhart-Hart Thermistor Equation:
 //    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]^3}
 //    where A = 0.001129148, B = 0.000234125 and C = 8.76741E-08
 long Resistance;  double Temp;  // Dual-Purpose variable to save space.
 Resistance=((10240000/RawADC) - 10000);  // Assuming a 10k Thermistor.  Calculation is actually: Resistance = (1024/ADC)
 Temp = log(Resistance); // Saving the Log(resistance) so not to calculate it 4 times later. // "Temp" means "Temporary" on this line.
 Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));   // Now it means both "Temporary" and "Temperature"
 Temp = Temp - 273.15;  // Convert Kelvin to Celsius                                         // Now it only means "Temperature"

 // BEGIN- Remove these lines for the function not to display anything
//  ////Serial.print("ADC: "); ////Serial.print(RawADC); ////Serial.print("/1024");  // Print out RAW ADC Number
//  ////Serial.print(", Volts: "); printDouble(((RawADC*4.860)/1024.0),3);   // 4.860 volts is what my USB Port outputs.
//  ////Serial.print(", Resistance: "); ////Serial.print(Resistance); ////Serial.print("ohms");
 // END- Remove these lines for the function not to display anything

 // Uncomment this line for the function to return Fahrenheit instead.
 //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert to Fahrenheit
 return Temp;  // Return the Temperature
}
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
    if(val >= 0) frac = (val - int(val)) * mult; else frac = (int(val) - val) * mult;
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
  double temp;
  int color[3];
  double d;
  int c;
  int i;

  temp=Thermistor(analogRead(ThermistorPIN));
  Serial.print("Temperature: ");
  printDouble(temp,3);
  Serial.println("C");


  if(temp<SCALE_MIN) {
    for(i=0;i<3;i++) 
      color[i]=colors[scale[0]+i];
  }else if(temp>SCALE_MAX){
    for(i=0;i<3;i++) 
      color[i]=colors[scale[SCALE_SIZE-1]+i];
  }else{
    d=(temp-SCALE_MIN)/(SCALE_MAX-SCALE_MIN)*(SCALE_SIZE-1.0);
    c=(int)d;
    d=d-(double)c;
    for(i=0;i<3;i++)
      color[i]=colors[scale[c]+i]+(int)((colors[scale[c+1]+i]-colors[scale[c]+i])*d);
  }
  analogWrite(PIN_RED, color[0]);
  analogWrite(PIN_GREEN, color[1]);
  analogWrite(PIN_BLUE, color[2]);
  delay(30);
}

