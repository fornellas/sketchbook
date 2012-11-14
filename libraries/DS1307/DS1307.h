#ifndef DS1307_h
#define DS1307_h

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#define SUNDAY 1
#define MONDAY 2
#define TUESDAY 3
#define WEDNESDAY 4
#define THRUSDAY 5
#define FRIDAY 6
#define SATURDAY 7

#define JANUARY 1
#define FEBRUARY 2
#define MARCH 3
#define APRIL 4
#define MAY 5
#define JUNE 6
#define JULY 7
#define AUGUST 8
#define SEPTEMBER 9
#define OCTOBER 10
#define NOVEMBER 11
#define DECEMBER 12

namespace DS1307 {
  struct Date {
    uint8_t second;
    uint8_t minute;
    uint8_t hour; // 24 Hour
    uint8_t weekDay; // 1-7 > sunday-saturday
    uint8_t monthDay;
    uint8_t month;
    uint16_t year;
  };
  struct DS1307::Date getDate();
  void setDate(struct DS1307::Date d);
  const PROGMEM char *getWeekDayName(uint8_t weekDay);
  const PROGMEM char *getMonthName(uint8_t month);
};

#endif

