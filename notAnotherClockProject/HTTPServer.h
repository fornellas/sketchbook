#ifndef HTTPServer_H
#define HTTPServer_H

#include <stdint.h>
#define WEBDUINO_NO_IMPLEMENTATION
#include <WebServer.h>

class HTTPServer {
private:
  static WebServer *webserver;
  // util
  static void footer(WebServer &server);
  // servers
  static void fileServer(WebServer &server, WebServer::ConnectionType type, char *path, bool tail_complete);
  static void status(WebServer &server, WebServer::ConnectionType type, char *path, bool tail_complete);
public:
  HTTPServer();
  static uint8_t loop();
};

#endif
