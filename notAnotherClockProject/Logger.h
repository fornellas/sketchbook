#ifndef Logger_h
#define Logger_h

#include <DS1307.h>
#include <avr/pgmspace.h>


#define LOGGER_UPDATE_MS (5*1000)

class Logger {
private:
  unsigned long lastLoggerUpdate;
  void fillBuff(PGM_P fileName, char *path, struct DS1307::Date *date, double value);
  void fillBuff(PGM_P fileName, char *path, struct DS1307::Date *date, int value);
public:
  Logger();
  void update();
};

#endif
