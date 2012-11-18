#include "Logger.h"

#include "pins.h"
#include "Light.h"
#include <Arduino.h>
#include <DS1307.h>
#include <avr/pgmspace.h>
#include <SD.h>
#include <BMP085.h>

extern Light *light;

#define BUFF_PATH 32
#define BUFF_FILE 13
#define PATH_SUFFIX 19

void Logger::update(){
  File file;
  // Path: DATA/AAAA/MM/DD/HH/12345678.txt
  char path[BUFF_PATH];
  char buff[BUFF_FILE];
  struct DS1307::Date date;

  analogWrite(PIN_R, light->read(255));
  // prefix
  date=DS1307::getDate();
  strcpy_P(path, PSTR("DATA/"));
  itoa(date.year, path+5, 10);
  path[9]='/';
  itoa(date.month, path+10, 10);
  path[12]='/';
  itoa(date.monthDay, path+13, 10);
  path[15]='/';
  itoa(date.hour, path+16, 10);
  path[18]='/';
//  if(!SD.exists(path))
    if(!SD.mkdir(path)){
      while(true)Serial.println("Error creating dir."); // FIXME better error reporting
    }
  // indoor temperature
  strcpy_P(path+PATH_SUFFIX, PSTR("TEMPINDO.TXT"));
  if(!(file=SD.open(path, FILE_WRITE))){
    while(true)Serial.println("Error opening file"); // FIXME better error reporting
  }
  itoa((uint16_t)date.minute*60+(uint16_t)date.second, buff, 10);
  uint8_t len;
  len=strlen(buff);
  buff[len]='\t';
  dtostrf(BMP085::readTemperature(), -2, 2, buff+len+1);
  len=strlen(buff);
  buff[len]='\n';
  buff[len+1]='\0';
  if(file.write(buff)!=len+1){
    while(true)Serial.println("Error writing to file"); // FIXME better error reporting
  }
  file.close();
  

  
  digitalWrite(PIN_R, LOW);
}
