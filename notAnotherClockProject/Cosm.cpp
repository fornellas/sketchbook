#include "Cosm.h"

#include <Arduino.h>
#include <math.h>
#include "CosmAPI.h"
#include "facilities.h"
#include "Weather.h"

#define UPDATE_MS (120L*1000L)

#define COSM_HOST "api.cosm.com"
#define COSM_PORT 80
#define PUT_PREFIX "PUT /v2/feeds/"
#define PUT_MIDDLE ".csv?key="
#define PUT_SUFFIX " HTTP/1.1"
#define SEND_BUFF_LEN 208
#define SEND_BUFF_END &sendBuff[strlen(sendBuff)]
#define APPEND_INT(i) itoa(i, SEND_BUFF_END, 10);
#define APPEND_STR(s) strcpy_P(SEND_BUFF_END, PSTR(s));
#define APPEND_DOUBLE(d) dtostrf(d, -2, 3, SEND_BUFF_END);
#define APPEND_CSV_DOUBLE(str, value) APPEND_STR(str); APPEND_DOUBLE(value); APPEND_STR("\n");

uint8_t Cosm::inProgress;
char Cosm::recvBuff[HTTP_200_LEN];
EthernetClient Cosm::client;

Cosm::Cosm(){
  lastUpdate=millis();
  inProgress=0;
  net->addProcessor(loop);
  
}

void Cosm::update(){
  if(!inProgress){
    if(millis()-lastUpdate>UPDATE_MS){
      char sendBuff[SEND_BUFF_LEN];
Serial.println("Cosm: New connection.");
      lastUpdate=millis();
      // new connection
      digitalWrite(PIN_G, LOW);
      analogWrite(PIN_R, light->read(255-5)+5);
      if(client.connect(strcpy_P(sendBuff, PSTR(COSM_HOST)), COSM_PORT)){
        inProgress=1;
        // PUT
        sendBuff[0]='\0';
        APPEND_STR(PUT_PREFIX);
        APPEND_STR(COSM_FEED);
        APPEND_STR(PUT_MIDDLE);
        APPEND_STR(COSM_KEY);
        APPEND_STR(PUT_SUFFIX);
//Serial.print("Cosm: ");
//Serial.println(sendBuff);
        client.println(sendBuff);
        // Host
        sendBuff[0]='\0';
        APPEND_STR("Host: ");
        APPEND_STR(COSM_HOST);
//Serial.print("Cosm: ");
//Serial.println(sendBuff); 
        client.println(sendBuff);
        // Content-Length begin
        sendBuff[0]='\0';
        APPEND_STR("Content-Length: ");
//Serial.print("Cosm: ");
//Serial.println(sendBuff);
        client.print(sendBuff);
        // Prepare PUT data
        sendBuff[0]='\0';
        bmp085->loadFromSensor();
        humidityInside.loadFromSensor();
        dht22->loadFromSensor();
        switch(dht22->error()){
          case DHT_ERROR_NONE:
          case DHT_ERROR_TOOQUICK:
            break;
          default:
            // FIXME better error reporting
            ledMatrix->fill(RED);
            ledMatrix->show();
            while(1);
            break;
        }
        APPEND_CSV_DOUBLE("temperatureOutside,", temperatureOutside.getC());
        APPEND_CSV_DOUBLE("temperatureInside,", temperatureInside.getC());
        APPEND_CSV_DOUBLE("humidityInside,", humidityInside.getRH());
        APPEND_CSV_DOUBLE("humidityOutside,", humidityOutside.getRH());
        APPEND_CSV_DOUBLE("dewPointInside,", Weather::dewPointC(humidityInside.getRH(), temperatureInside.getC()));
        APPEND_CSV_DOUBLE("dewPointOutside,", Weather::dewPointC(humidityOutside.getRH(), temperatureOutside.getC()));
        APPEND_CSV_DOUBLE("heatIndexInside,", Weather::heatIndexC(humidityInside.getRH(), temperatureInside.getC()));
        APPEND_CSV_DOUBLE("heatIndexOutside,", Weather::heatIndexC(humidityOutside.getRH(), temperatureOutside.getC()));
        APPEND_CSV_DOUBLE("pressure,", pressure.getPa());
        // Content-Length value
//Serial.print("Cosm: ");
//Serial.println(strlen(sendBuff));
        client.println(strlen(sendBuff));
        // Headers ending
        client.println();
        // Send PUT data
//Serial.print("Cosm: ");
//Serial.println(sendBuff);
        client.print(sendBuff);
        // Finish
        recvBuff[0]='\0';
      }else{
        // FIXME better error report
Serial.println("Cosm: Failed to open new connection.");
        lastUpdate=millis();
      }
      analogWrite(PIN_G, light->read(255-5)+5);
      digitalWrite(PIN_R, LOW);
    }
  }else{
    loop();
  }
}

uint8_t Cosm::loop(){
  if(inProgress){
Serial.println("Cosm: inProgress");
    if(client.connected()){
//Serial.println("Cosm: connected");
      digitalWrite(PIN_G, LOW);
      analogWrite(PIN_R, light->read(255-5)+5);
      // Headers
//Serial.println("Cosm: Headers");
      while(client.available()){
        char c;
        c=client.read();
//Serial.print("Cosm: New byte: '");
//Serial.print(c);
//Serial.println("'.");
        // fill buffer
        uint8_t l;
        l=strlen(recvBuff);
        if(l<HTTP_200_LEN){
          recvBuff[l]=c;
          recvBuff[l+1]='\0';
//Serial.print("Cosm: Buffer: '");
//Serial.print(recvBuff);
//Serial.println("'");
          if(l+2==HTTP_200_LEN) {
            // buffer full
//Serial.print("Cosm: Buffer full: ");
//Serial.println(recvBuff);
            char success[HTTP_200_LEN];
            strcpy_P(success, PSTR("HTTP/1.1 200 OK"));
            // processing OK
            if(!strcmp(recvBuff, success)){
Serial.println("Cosm: Processing OK.");
            // Error response
            }else{
              // FIXME Better error reporting
Serial.println("Cosm: Unexpected response:");
Serial.println(recvBuff);
            }
            inProgress=0;
            client.stop();
          }
        }
      }
      analogWrite(PIN_G, light->read(255-5)+5);
      digitalWrite(PIN_R, LOW);
    }else{
Serial.println("Cosm: Connection lost before end of transaciton.");
      // connection lost before end of transaciton.
      // FIXME better error reporting
      inProgress=0;
      client.stop();
    }
  }
}
