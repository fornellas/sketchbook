#include "Logger.h"

#include "Light.h"
#include <Arduino.h>
#include <DS1307.h>
#include <avr/pgmspace.h>
#include <SD.h>
#include <BMP085.h>
#include "pins.h"


extern Light *light;

#define BUFF_PATH 32
#define BUFF_FILE 13
#define BUFF_VALUE 8
#define PATH_SUFFIX 19

Logger::Logger(){
  lastLoggerUpdate=millis();
}

void Logger::update(){
  if(millis()-lastLoggerUpdate>LOGGER_UPDATE_MS){
    File file;
    uint8_t len;
    // Path: DATA/AAAA/MM/DD/HH/12345678.txt
    char path[BUFF_PATH];
    char buff[BUFF_FILE];
    struct DS1307::Date date;

    analogWrite(PIN_R, light->read(255));
    lastLoggerUpdate+=LOGGER_UPDATE_MS;
    // prefix
    date=DS1307::getDate();
    strcpy_P(path, PSTR("DATA/"));
    itoa(date.year, path+5, 10);
    path[9]='/';
    if(date.month<10){
      path[10]='0';
      itoa(date.month, path+11, 10);
    }else
      itoa(date.month, path+10, 10);
    path[12]='/';
    if(date.monthDay<10){
      path[13]='0';
      itoa(date.monthDay, path+14, 10);
    }else
      itoa(date.monthDay, path+13, 10);
    path[15]='/';
    if(date.hour<10){
      path[16]='0';
      itoa(date.hour, path+17, 10);
    }else
      itoa(date.hour, path+16, 10);
    path[18]='/';
    path[19]='\0';
    if(!SD.mkdir(path)){
      while(true)Serial.println("Error creating dir."); // FIXME better error reporting
    }
    // indoor temperature
    strcpy_P(path+PATH_SUFFIX, PSTR("TEMPINDO.TXT"));
    itoa((uint16_t)date.minute*60+(uint16_t)date.second, buff, 10);
    len=strlen(buff);
    buff[len]='\t';
    dtostrf(BMP085::readTemperature(), -2, 2, buff+len+1);
    len=strlen(buff);
    buff[len]='\n';
    buff[len+1]='\0';
    if(!(file=SD.open(path, FILE_WRITE))){
      while(true)Serial.println("Error opening file"); // FIXME better error reporting
    }
    if(file.write(buff)!=len+1){
      while(true)Serial.println("Error writing to file"); // FIXME better error reporting
    }
    file.close();

    
    digitalWrite(PIN_R, LOW);
  }
}
