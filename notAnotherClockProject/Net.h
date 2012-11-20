#ifndef Net_h
#define Net_h

#include <stdint.h>

class Net {
private:
  uint8_t (**processor)();
  uint8_t processorCount;
public:
  Net();
  void processAll();
  void addProcessor(uint8_t (*l)());
  void removeProcessor(uint8_t (*l)());
  ~Net();
};

#endif

