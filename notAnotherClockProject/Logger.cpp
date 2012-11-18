#include "Logger.h"

#include "Light.h"
#include <Arduino.h>
#include <DS1307.h>
#include <avr/pgmspace.h>
#include <SD.h>
#include <BMP085.h>
#include "pins.h"
#include <DS18S20.h>
#include <HIH4030.h>

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
    // Path: DATA/AAAA/MM/DD/HH/12345678.txt
    char path[BUFF_PATH];
    struct DS1307::Date date;

    analogWrite(PIN_R, light->read(255-20)+20);
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
    // Indoor Temperature
    float temperatureInside;
     byte addr[]={40, 200, 10, 228, 3, 0, 0, 62};
    save(PSTR("TEMPINDO.TXT"), path, &date, temperatureInside=BMP085::readTemperature());
    // Outdoor Temperature
    save(PSTR("TEMPOUTD.TXT"), path, &date, DS18S20::read(PIN_TEMP_EXT, addr));
    // Humidity
    save(PSTR("HUMIDITY.TXT"), path, &date, HIH4030::read(PIN_HUMIDITY, temperatureInside));
    save(PSTR("PRESSURE.TXT"), path, &date, BMP085::readPressure());
    
    digitalWrite(PIN_R, LOW);
  }
}

// Append file name to path, add date\t to txt
#define FILL_BUFF(fileName, path, date, value) \
  char txt[BUFF_FILE]; \
  uint8_t len; \
  strcpy_P(path+PATH_SUFFIX, fileName); \
  itoa((uint16_t)date->minute*60+(uint16_t)date->second, txt, 10); \
  len=strlen(txt); \
  txt[len]='\t';

// save values to SD
void Logger::save(PGM_P fileName, char *path, struct DS1307::Date *date, double value){
  FILL_BUFF(fileName, path, date, value);
  dtostrf(value, -2, 2, txt+len+1);
  writeToFile(path, txt);
}
void Logger::save(PGM_P fileName, char *path, struct DS1307::Date *date, int value){
  FILL_BUFF(fileName, path, date, value);
  itoa(value, txt+len+1, 10);
  writeToFile(path, txt);
}
void Logger::save(PGM_P fileName, char *path, struct DS1307::Date *date, long int value){
  FILL_BUFF(fileName, path, date, value);
  ltoa(value, txt+len+1, 10);
  writeToFile(path, txt);
}

// append \n and save
void Logger::writeToFile(char *path, char *txt){
  uint8_t len;
  File file;

  len=strlen(txt);
  txt[len]='\n';
  txt[len+1]='\0';
  if(!(file=SD.open(path, FILE_WRITE))){
    while(true)Serial.println("Error opening file"); // FIXME better error reporting
  }
  if(file.write(txt)!=len+1){
    while(true)Serial.println("Error writing to file"); // FIXME better error reporting
  }
  file.close();
}
