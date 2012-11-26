#ifndef Logger_h
#define Logger_h

#include <Time.h>
#include <avr/pgmspace.h>


#define LOGGER_UPDATE_MS (120*1000)

class Logger {
private:
  unsigned long lastLoggerUpdate;
  void writeToFile(char *path, char *txt);
  void save(PGM_P fileName, char *path, Time *time, double value);
  void save(PGM_P fileName, char *path, Time *time, int value);
  void save(PGM_P fileName, char *path, Time *time, long int value);
public:
  Logger();
  void update();
};

#endif
