#ifndef Net_h
#define Net_h

#include <stdint.h>

class Net {
private:
  void (**processor)();
  uint8_t processorCount;
public:
  Net();
  void processAll();
  void addProcessor(void (*l)());
  ~Net();
};

#endif













