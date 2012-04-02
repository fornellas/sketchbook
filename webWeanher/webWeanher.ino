#include <OneWire.h>

#include <Wire.h>

#include <SPI.h>
#include <Ethernet.h>

#include <DS18S20.h>
#include <HIH4030.h>
#include <BMP085.h>
#include <DS1307.h>

#define PIN_HIH4030 A3
#define MY_ALTITUDE 810.0

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  HIH4030::setup(PIN_HIH4030);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.println("Server ready!");
}

void loop()
{
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New client.");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      Serial.println("Client connected.");
      if (client.available()) {
        Serial.println("Client available.");
        char c = client.read();

        Serial.print("Read: '");
        Serial.print(c);
        Serial.println("'");
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          Serial.println("Sending response.");

          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/plain");
          client.println();

          client.println("***********************");
          client.println("* Arduino Web Weather *");
          client.println("***********************");
          client.println("");

          struct DS1307::Date d;
          d=DS1307::getDate();
          client.print("Local time:\t\t");
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

          client.print("Temperature indoor:\t");
          client.print(BMP085::readTemperature());
          client.println(" C");

          byte addr[]={
            40, 200, 10, 228, 3, 0, 0, 62                                                                                                                                                                                                        };
          client.print("Temperature outdoor:\t");
          client.print(DS18S20::read(2, addr));
          client.println(" C");

          client.print("Humidity:\t\t");
          client.print(HIH4030::read(PIN_HIH4030, BMP085::readTemperature()));
          client.println("%");

          long pressure;
          pressure=BMP085::readPressure();

          client.print("Pressure:\t\t");
          client.print(pressure/100);
          client.println(" hPa");

          const float p0 = 101325;     // Pressure at sea level (Pa)
          // Add these to the top of your program
          const float currentAltitude = MY_ALTITUDE; // current altitude in METERS
          const float ePressure = p0 * pow((1-currentAltitude/44330), 5.255);  // expected pressure (in Pa) at altitude

          client.print("Expected pressure:\t");
          client.print((long)ePressure/100);
          client.println(" hPa");

          int16_t weatherDiff;

          // Add this into loop(), after you've calculated the pressure
          weatherDiff = (int16_t)pressure - (int16_t)ePressure;

          client.print("Pressure difference:\t");
          client.print(weatherDiff/100);
          client.println(" hPa");

          client.print("Weather:\t\t");
          if(weatherDiff > 250)
            client.println("clear.");
          else if ((weatherDiff <= 250) || (weatherDiff >= -250))
            client.println("partly cloudy.");
          else if (weatherDiff > -250)
            client.println("rain.");

          double t = 0;
          t = (double) pressure/101325;
          t = 1-pow(t, 0.19029);
          client.print("Absolute altitude:\t");
          client.print(round(44330.0*t));
          client.println(" m");

          client.print("GPS altitude:\t\t");
          client.print((int)MY_ALTITUDE);
          client.println(" m");

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
    Serial.println("Disconnected.");
  }
}








