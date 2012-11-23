#ifndef Time_h
#define Time_h

#include <inttypes.h>
#include <avr/pgmspace.h>

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

#define UTC 0

typedef int32_t time_t;

typedef struct {
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint8_t mday;
  uint8_t mon;
  uint16_t year;
  uint8_t wday;
  int8_t zone;
} btime_t;

class Time{
protected:
  btime_t bTime;
public:
  Time(int8_t zone);
  // RTC
  virtual void loadFromRTC(){}; // should be pure virtual, but avr-gcc can not link it...
  virtual void saveToRTC()=0;
  // change
  void mktime(btime_t newBTime);
  void mktime(time_t newLocalTime);
  void changeZone(int8_t newZone);
  // return values
  time_t getLocalTime();
  btime_t getBTime();
  // names
  const PROGMEM char *getWeekDayName(uint8_t weekDay);
  const PROGMEM char *getWeekDayName();
  const PROGMEM char *getMonthName(uint8_t month);
  const PROGMEM char *getMonthName();
};

#endif
