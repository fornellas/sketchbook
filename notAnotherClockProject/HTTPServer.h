#ifndef HTTPServer_H
#define HTTPServer_H

#include <stdint.h>
#define WEBDUINO_NO_IMPLEMENTATION
#include <WebServer.h>

namespace HTTPServer {
  void begin();
  void loop();
}

#endif
