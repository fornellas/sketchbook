#ifndef Logger_h
#define Logger_h

#include <DS1307.h>
#include <avr/pgmspace.h>


#define LOGGER_UPDATE_MS (30*1000)

class Logger {
private:
  unsigned long lastLoggerUpdate;
  void writeToFile(char *path, char *txt);
  void save(PGM_P fileName, char *path, struct DS1307::Date *date, double value);
  void save(PGM_P fileName, char *path, struct DS1307::Date *date, int value);
  void save(PGM_P fileName, char *path, struct DS1307::Date *date, long int value);
public:
  Logger();
  void update();
};

#endif