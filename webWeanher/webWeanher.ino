// http://www.sparkfun.com/tutorial/Barometric/BMP085_Example_Code.pde
/* BMP085 Extended Example Code
 by: Jim Lindblom
 SparkFun Electronics
 date: 1/18/11
 license: CC BY-SA v3.0 - http://creativecommons.org/licenses/by-sa/3.0/
 
 Get pressure and temperature from the BMP085 and calculate altitude.
 Serial.print it out at 9600 baud to serial monitor.
 
 Update (7/19/11): I've heard folks may be encountering issues
 with this code, who're running an Arduino at 8MHz. If you're 
 using an Arduino Pro 3.3V/8MHz, or the like, you may need to 
 increase some of the delays in the bmp085ReadUP and 
 bmp085ReadUT functions.
 */

#define MY_ALTITUDE 810.0

#include <Wire.h>
#include <FreqCounter.h>
#include <SPI.h>
#include <Ethernet.h>
#include <RTCDS1307.h>
#include "Wire.h"
RTCDS1307 *rtc;
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1, 4);

EthernetServer server(80);

int freq;
int offset=7753;
int sens=398;

#define BMP085_ADDRESS 0x77  // I2C address of BMP085

const unsigned char OSS = 3;  // Oversampling Setting

// Calibration values
int ac1;
int ac2; 
int ac3; 
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1; 
int b2;
int mb;
int mc;
int md;

// b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
// so ...Temperature(...) must be called before ...Pressure(...).
long b5; 

short temperature;
long pressure;

void setup()
{
  rtc=new RTCDS1307();
  Serial.begin(115200);
  Wire.begin();
  bmp085Calibration();
  Ethernet.begin(mac, ip);
  server.begin();
      Serial.println("Server ready!");
}

void loop()
{
  struct Date d;
  //   Serial.println("loop()");
  EthernetClient client = server.available();
  //   Serial.println("server.available() passed");
  if (client) {
    Serial.println("We have got a client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      //       Serial.println("Still connected...");
      if (client.available()) {
        //         Serial.println("client.available()");
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          Serial.println("Sending response...");
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/plain");
          client.println();
          client.flush();

          client.println("***********************");
          client.println("* Arduino Web Weather *");
          client.println("***********************");
          client.println("");
          client.flush();

          long altitude = 0;
          double temp = 0;

          FreqCounter::f_comp= 8;             // Set compensation to 12
          FreqCounter::start(1000);            // Start counting with gatetime of 1000ms
          while (FreqCounter::f_ready == 0)         // wait until counter ready
            freq=FreqCounter::f_freq;            // read result
          float RH =  (double(offset)-double(freq))*double(sens)/double(4096); //Sure, you can use int - depending on what do you need

          d=rtc->getDate();
          client.print("Local time: ");
          client.print(d.year);
          client.print("-");
          client.print(d.month);
          client.print("-");
          client.print(d.monthDay);
          client.print(" ");
          client.print(d.hour);
          client.print(":");
          client.print(d.minute);
          client.print(":");
          client.println(d.second);
          client.flush();

          temperature = bmp085GetTemperature(bmp085ReadUT());
          pressure = bmp085GetPressure(bmp085ReadUP());

          client.print("Temperature: ");
          client.print(double(temperature)/10);
          client.println(" C");
          client.flush();

          client.print("Humidity: ");
          client.print(RH);
          client.println("%");
          client.flush();

          client.print("Pressure: ");
          client.print(pressure, DEC);
          client.println(" Pa");
          client.flush();

          temp = (double) pressure/101325;
          temp = 1-pow(temp, 0.19029);
          altitude = round(44330*temp);
          client.print("Altitude: ");
          client.print(altitude);
          client.println("m");
          client.flush();

          const float p0 = 101325;     // Pressure at sea level (Pa)
          // Add these to the top of your program
          const float currentAltitude = MY_ALTITUDE; // current altitude in METERS
          const float ePressure = p0 * pow((1-currentAltitude/44330), 5.255);  // expected pressure (in Pa) at altitude
          /*
          client.print("Expected pressure: ");
           client.print(ePressure);
           client.println(" Pa");
           client.println("<br />");
           */
          float weatherDiff;

          // Add this into loop(), after you've calculated the pressure
          weatherDiff = pressure - ePressure;
          /*
          client.print("Pressure difference: ");
           client.println(weatherDiff);
           client.println("<br />");
           */
          client.print("Weather: ");
          if(weatherDiff > 250)
            client.println("clear.");
          else if ((weatherDiff <= 250) || (weatherDiff >= -250))
            client.println("partly cloudy.");
          else if (weatherDiff > -250)
            client.println("rain.");
          client.println(); 

          //         Serial.println("flushing...");

          client.flush();


          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Finished!");
  }
  //   Serial.println("loop() end");
}

// Stores all of the bmp085's calibration values into global variables
// Calibration values are required to calculate temp and pressure
// This function should be called at the beginning of the program
void bmp085Calibration()
{
  ac1 = bmp085ReadInt(0xAA);
  ac2 = bmp085ReadInt(0xAC);
  ac3 = bmp085ReadInt(0xAE);
  ac4 = bmp085ReadInt(0xB0);
  ac5 = bmp085ReadInt(0xB2);
  ac6 = bmp085ReadInt(0xB4);
  b1 = bmp085ReadInt(0xB6);
  b2 = bmp085ReadInt(0xB8);
  mb = bmp085ReadInt(0xBA);
  mc = bmp085ReadInt(0xBC);
  md = bmp085ReadInt(0xBE);
}

// Calculate temperature given ut.
// Value returned will be in units of 0.1 deg C
short bmp085GetTemperature(unsigned int ut)
{
  long x1, x2;

  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8)>>4);  
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
long bmp085GetPressure(unsigned long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;

  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;

  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;

  return p;
}

// Read 1 byte from the BMP085 at 'address'
char bmp085Read(unsigned char address)
{
  unsigned char data;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available())
    ;

  return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int bmp085ReadInt(unsigned char address)
{
  unsigned char msb, lsb;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.read();
  lsb = Wire.read();

  return (int) msb<<8 | lsb;
}

// Read the uncompensated temperature value
unsigned int bmp085ReadUT()
{
  unsigned int ut;

  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();

  // Wait at least 4.5ms
  delay(5);

  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

// Read the uncompensated pressure value
unsigned long bmp085ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;

  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();

  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));

  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);

  // Wait for data to become available
  while(Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();

  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);

  return up;
}



















