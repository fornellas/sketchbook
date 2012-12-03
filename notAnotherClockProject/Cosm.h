#ifndef Cosm_h
#define Cosm_h

#include <inttypes.h>
#include <Ethernet.h>

#define HTTP_200_LEN 16 // "HTTP/1.1 200 OK"

class Cosm {
private:
  unsigned long lastUpdate;
  static uint8_t inProgress;
  static char recvBuff[HTTP_200_LEN];
  static EthernetClient client;
public:
  Cosm();
  void update();
  static uint8_t loop();
};

#endif
