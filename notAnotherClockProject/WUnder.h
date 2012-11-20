#ifndef WUnder_h
#define WUnder_h

#include <inttypes.h>
#include <Ethernet.h>

#define SUCCESS_LENGTH 8

class WUnder {
private:
  unsigned long lastUpdate;
  static uint8_t inProgress;
  static uint8_t skipHeaders;
  static char lastChar;
  static char recvBuff[SUCCESS_LENGTH];
  static EthernetClient client;
public:
  WUnder();
  void update();
  static uint8_t loop();
};

#endif
