#include <OneWire.h>

#include <Wire.h>

#include <SPI.h>
#include <Ethernet.h>

#include <DS18S20.h>
#include <HIH4030.h>
#include <BMP085.h>
#include <DS1307.h>

#define PIN_HIH4030 A3

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

          struct DS1307::Date d;
          d=DS1307::getDate();
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

          client.print("Temperature indoor: ");
          client.print(BMP085::readTemperature());
          client.println(" C");
          client.flush();

          byte addr[]={
            40, 200, 10, 228, 3, 0, 0, 62                                                                                                                                            };
          client.print("Temperature outdoor: ");
          client.print(DS18S20::read(2, addr));
          client.println(" C");
          client.flush();

          client.print("Humidity: ");
          client.print(HIH4030::read(PIN_HIH4030, BMP085::readTemperature()));
          client.println("%");
          client.flush();

          long pressure;
          client.print("Pressure: ");
          client.print(pressure=BMP085::readPressure());
          client.println(" Pa");
          client.flush();

          double t = 0;
          t = (double) pressure/101325;
          t = 1-pow(t, 0.19029);
          client.print("Altitude: ");
          client.print(round(44330*t));
          client.println("m");
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


