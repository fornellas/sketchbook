#ifndef RTCDS1307_h
#define RTCDS1307_h

#include <Arduino.h>

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

struct date {
  int second;
  int minute;
  int hour; // 24 Hour
  int weekDay; // 1-7 > sunday-saturday
  int monthDay;
  int month;
  int year;
};

class RTCDS1307
{
  private:
    byte bcdToDec(byte val);
    byte decToBcd(byte val);
  public:
    RTCDS1307(bool);
    struct date getDate();
    void setDate(struct date d);
};

#endif

