#ifndef DS1307_h
#define DS1307_h

#include <Time.h>

class DS1307:
public Time {
private:
  uint8_t bcdToDec(uint8_t val);
  uint8_t decToBcd(uint8_t val);
public:
  btime_t readBTimeRTC();
  void setRTC(btime_t newBTime);
  DS1307(int8_t zone);
};

#endif

