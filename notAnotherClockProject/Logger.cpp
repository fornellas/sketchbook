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
extern DS18S20 *temperatureOutside;
extern BMP085 *pressure;
extern DS1307 *time;

#define BUFF_PATH 32
#define BUFF_FILE 19
#define BUFF_VALUE 8
#define PATH_SUFFIX 19

Logger::Logger(){
  lastLoggerUpdate=millis();
}

void Logger::update(){
  if(millis()-lastLoggerUpdate>LOGGER_UPDATE_MS){
    // Path: DATA/AAAA/MM/DD/HH/12345678.txt
    char path[BUFF_PATH];
    btime_t date;

    analogWrite(PIN_R, light->read(255-20)+20);
    lastLoggerUpdate+=LOGGER_UPDATE_MS;
    // prefix
    date=time->readBTimeRTC();
    time->changeZone(0); // always use GMT
    strcpy_P(path, PSTR("DATA/"));
    itoa(date.year, path+5, 10);
    path[9]='/';
    if(date.mon<10){
      path[10]='0';
      itoa(date.mon, path+11, 10);
    }else
      itoa(date.mon, path+10, 10);
    path[12]='/';
    if(date.mday<10){
      path[13]='0';
      itoa(date.mday, path+14, 10);
    }else
      itoa(date.mday, path+13, 10);
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
    save(PSTR("TEMPINDO.TXT"), path, &date, pressure->readC());
    // Outdoor Temperature
    save(PSTR("TEMPOUTD.TXT"), path, &date, temperatureOutside->readC());
    // Humidity
    save(PSTR("HUMIDITY.TXT"), path, &date, HIH4030::read(PIN_HUMIDITY, temperatureInside));
    save(PSTR("PRESSURE.TXT"), path, &date, (long int)pressure->readPa());
    
    digitalWrite(PIN_R, LOW);
  }
}

// Append file name to path, add date\t to txt
#define FILL_BUFF(fileName, path, date, value) \
  char txt[BUFF_FILE]; \
  uint8_t len; \
  strcpy_P(path+PATH_SUFFIX, fileName); \
  ltoa(time->getLocalTime(), txt, 10); \
  len=strlen(txt); \
  txt[len]='\t';

// save values to SD
void Logger::save(PGM_P fileName, char *path, btime_t *date, double value){
  FILL_BUFF(fileName, path, date, value);
  dtostrf(value, -2, 2, txt+len+1);
  writeToFile(path, txt);
}
void Logger::save(PGM_P fileName, char *path, btime_t *date, int value){
  FILL_BUFF(fileName, path, date, value);
  itoa(value, txt+len+1, 10);
  writeToFile(path, txt);
}
void Logger::save(PGM_P fileName, char *path, btime_t *date, long int value){
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
