#ifndef DS1307_h
#define DS1307_h

#include <Time.h>

class DS1307:
public Time {
private:
  uint8_t bcdToDec(uint8_t val);
  uint8_t decToBcd(uint8_t val);
public:
  DS1307(int8_t zone);
  void loadFromRTC();
  void saveToRTC();
};

#endif
