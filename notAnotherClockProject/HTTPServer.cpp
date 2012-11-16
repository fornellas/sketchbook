#define WEBDUINO_NO_IMPLEMENTATION
#include "HTTPServer.h"

WebServer webserver("", 80);

void helloCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type != WebServer::HEAD)
  {
    /* this defines some HTML text in read-only memory aka PROGMEM.
     * This is needed to avoid having the string copied to our limited
     * amount of RAM. */
    P(helloMsg) = "<h1>Hello, World!</h1>";

    /* this is a special form of print that outputs from PROGMEM */
    server.printP(helloMsg);
  }
}

void HTTPServer::begin(){
  webserver.setDefaultCommand(&helloCmd);
  webserver.begin();
}

void HTTPServer::loop(){
  webserver.processConnection();
  while(webserver.available())
    webserver.processConnection();
}
