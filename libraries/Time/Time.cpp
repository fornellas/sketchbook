#include <Time.h>

#define EPOCH 1970L
#define HOURS_PER_DAY 24L
#define SECS_PER_MIN 60L
#define MIN_PER_HOUR 60L
#define SECS_PER_HOUR (MIN_PER_HOUR*SECS_PER_MIN)
#define SECS_PER_DAY (HOURS_PER_DAY*SECS_PER_HOUR)
#define DAYS_PER_YEAR 365L
#define SECS_PER_YEAR (SECS_PER_DAY*DAYS_PER_YEAR)
#define SECS_30_DAYS (30L*SECS_PER_DAY)
#define SECS_28_DAYS (28L*SECS_PER_DAY)
#define SECS_31_DAYS (31L*SECS_PER_DAY)
#define SECS_FEB SECS_31_DAYS
#define SECS_MAR (SECS_FEB+SECS_28_DAYS)
#define SECS_APR (SECS_MAR+SECS_31_DAYS)
#define SECS_MAY (SECS_APR+SECS_30_DAYS)
#define SECS_JUN (SECS_MAY+SECS_31_DAYS)
#define SECS_JUL (SECS_JUN+SECS_30_DAYS)
#define SECS_AUG (SECS_JUL+SECS_31_DAYS)
#define SECS_SEP (SECS_AUG+SECS_31_DAYS)
#define SECS_OCT (SECS_SEP+SECS_30_DAYS)
#define SECS_NOV (SECS_OCT+SECS_31_DAYS)
#define SECS_DEC (SECS_NOV+SECS_30_DAYS)
#define SECS_PER_4_YEARS (SECS_PER_YEAR*4+SECS_PER_DAY)

Time::Time(int8_t zone){
  bTime.zone=zone;
  loadFromRTC();
}

// change

void Time::mktime(btime_t newBTime){
  bTime=newBTime;
}

void Time::mktime(time_t newLocalTime){
  time_t t;
  // week day
  bTime.wday=((newLocalTime/SECS_PER_DAY)+4)%7+1;
  // years up to previous leap year
  bTime.year=newLocalTime/SECS_PER_4_YEARS*4+EPOCH;
  newLocalTime-=(bTime.year-EPOCH)*SECS_PER_YEAR+((bTime.year+1L-EPOCH)/4L)*SECS_PER_DAY;
  // years from previous leap year until start of current year
  t=newLocalTime/SECS_PER_YEAR; // years
  bTime.year+=t;
  newLocalTime-=t*SECS_PER_YEAR;
  if(t>2)
    newLocalTime-=SECS_PER_DAY;
  // month
  bTime.mon=1;
  if(bTime.year%4)
    t=0;
  else
    t=SECS_PER_DAY;
#define MON(name, number) if(newLocalTime>name+t){bTime.mon=number;newLocalTime-=name+t;goto next;}
  MON(SECS_DEC, 12);
  MON(SECS_NOV, 11);
  MON(SECS_OCT, 10);
  MON(SECS_SEP, 9);
  MON(SECS_AUG, 8);
  MON(SECS_JUL, 7);
  MON(SECS_JUN, 6);
  MON(SECS_MAY, 5);
  MON(SECS_APR, 4);
  MON(SECS_MAR, 3);
  if(newLocalTime>SECS_FEB){
    bTime.mon=2;
    goto next;
  }
  bTime.mon=1;
  next:
  // set mday, hour, min and sec
#define SET(x, s) bTime.x=newLocalTime/s; newLocalTime%=s;
  SET(mday, SECS_PER_DAY);
  bTime.mday++;
  SET(hour, SECS_PER_HOUR);
  SET(min, SECS_PER_MIN);
  bTime.sec=newLocalTime;
}

void Time::changeZone(int8_t newZone){
  mktime(getLocalTime()+((time_t)newZone-(time_t)bTime.zone)*SECS_PER_HOUR);
  bTime.zone=newZone;
}

// return values

btime_t Time::getBTime(){
  return bTime;
}

time_t Time::getLocalTime(){
  time_t t;

  t=0;
  // seconds up to 1 Jan of the year
  t+=(bTime.year-EPOCH)*SECS_PER_YEAR;
  t+=((bTime.year+1L-EPOCH)/4L)*SECS_PER_DAY; // sum leap days
  // Sum seconds up to 1 of current month
  switch(bTime.mon){
    case 2:
      t+=SECS_FEB;
      break;
    case 3:
      t+=SECS_MAR;
      break;
    case 4:
      t+=SECS_APR;
      break;
    case 5:
      t+=SECS_MAY;
      break;
    case 6:
      t+=SECS_JUN;
      break;
    case 7:
      t+=SECS_JUL;
      break;
    case 8:
      t+=SECS_AUG;
      break;
    case 9:
      t+=SECS_SEP;
      break;
    case 10:
      t+=SECS_OCT;
      break;
    case 11:
      t+=SECS_NOV;
      break;
    case 12:
      t+=SECS_DEC;
      break;
  }
  // if leap year and month >2, add leap day
  if(!(bTime.year%4)&&bTime.mon>2)
    t+=SECS_PER_DAY;
  // Sum days
  t+=(bTime.mday-1)*SECS_PER_DAY;
  // Sum hours
  t+=bTime.hour*SECS_PER_HOUR;
  // Sum minutes
  t+=bTime.min*SECS_PER_MIN;
  // Sum seconds
  t+=bTime.sec;
  return t;
}

// names

const PROGMEM char *Time::getWeekDayName(uint8_t weekDay){
  switch(weekDay){
    case 1:
      return PSTR("Sunday");
    case 2:
      return PSTR("Monday");
    case 3:
      return PSTR("Tuesday");
    case 4:
      return PSTR("Wednesday");
    case 5:
      return PSTR("Thrusday");
    case 6:
      return PSTR("Friday");
    case 7:
      return PSTR("Saturday");
  }
}

const PROGMEM char *Time::getWeekDayName(){
  return getWeekDayName(bTime.wday);
}

const PROGMEM char *Time::getMonthName(uint8_t month){
  switch(month){
    case 1:
      return PSTR("January");
    case 2:
      return PSTR("February");
    case 3:
      return PSTR("March");
    case 4:
      return PSTR("April");
    case 5:
      return PSTR("May");
    case 6:
      return PSTR("June");
    case 7:
      return PSTR("July");
    case 8:
      return PSTR("August");
    case 9:
      return PSTR("September");
    case 10:
      return PSTR("October");
    case 11:
      return PSTR("November");
    case 12:
      return PSTR("December");
  }
}

const PROGMEM char *Time::getMonthName(){
  return getMonthName(bTime.mon);
}

