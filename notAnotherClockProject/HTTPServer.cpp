#include "HTTPServer.h"
#include <SD.h>
#include <MemoryFree.h>
#include <DS1307.h>
#include "facilities.h"

#define BUFF_READ 64
#define BUFF_TAIL 37
#define BUFF_MIME 26

#define HTTP_PORT 80

WebServer *HTTPServer::webserver=NULL;

void HTTPServer::footer(WebServer &server){
  time_t uptime;
  DS1307 time(UTC);

  uptime=time.getLocalTime()-bootTime;
  server.printP(PSTR("notAnotherClockProject Server at port "));
  server.print(HTTP_PORT);
  server.printP(PSTR(".<br>Compiled on: "));
  server.printP(PSTR(__DATE__));
  server.printP(PSTR("<br>Uptime: "));
  server.print(uptime/(3600L*24L));
  server.printP(PSTR(" day(s), "));
  server.print((uptime%(3600L*24L))/3600L);
  server.printP(PSTR(" hour(s), "));
  server.print((uptime%(3600L))/60L);
  server.printP(PSTR(" minute(s)."));
}

void HTTPServer::fileServer(WebServer &server, WebServer::ConnectionType type, char *path, bool tail_complete){
  File file;

  // only GET
  if (type != WebServer::GET){
    server.httpFail();
    return;
  }
  // small buffer 
  if(!tail_complete){
    server.httpFail();
    server.printP(PSTR("URL buffer is not big enough."));
    return;
  }
  // no path
  if(!strlen(path)){
    server.httpFail();
    server.printP(PSTR("Missing file path."));
    return;
  }
  // Missing leading /
  if(path[0]!='/'){
    server.httpFail();
    server.printP(PSTR("Path must begin with /"));
    return;
  }
  // File not found
  if(path[1]!='\0')
    if(!SD.exists(path)){
      server.httpFail();
      server.printP(PSTR("File not found."));
      return;
    }

  // open
  if(!(file=SD.open(path))){
    server.httpFail();
    server.printP(PSTR("Error opening file."));
    return;
  }

  // list directories
  if(file.isDirectory()){
    server.httpSuccess();
    server.printP(PSTR("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n<html>\n<title>Arduino File Server</title>\n<body>\n<h1>Index of "));
    server.print(path);
    server.printP(PSTR("</h1>\n<table border=\"1\">\n<tr><td><b>Type</b></td><td><b>Name</b></td><td><b>Size</b></td></tr>"));
    file.rewindDirectory();
    while(File entry=file.openNextFile()){
      // Type
      server.printP(PSTR("<tr><td>"));
      if(entry.isDirectory())
        server.printP(PSTR("DIR"));
      else
        server.printP(PSTR("FILE"));
      // Name
      server.printP(PSTR("</td><td>"));
      server.printP(PSTR("<a href=\"sd?"));
      server.print(path);
      if(path[strlen(path)-1]!='/')
        server.printP(PSTR("/"));
      server.print(entry.name());
      server.printP(PSTR("\">"));
      server.print(entry.name());
      // Size
      server.printP(PSTR("</a></td><td>"));
      server.print(entry.size());
      entry.close();
      server.printP(PSTR("</td></tr>\n"));
    }
    server.printP(PSTR("</tr>\n</table>\n<hr>\n"));
    footer(server);
    server.printP(PSTR("</body>\n</html>\n"));
  // dump files
  } else {
    char readBuffer[BUFF_READ];
    char mimeBuffer[BUFF_MIME];
    server.httpSuccess(strcpy_P(mimeBuffer, PSTR("text/plain; charset=utf-8")), NULL);
    while(uint8_t size=file.read(readBuffer, BUFF_READ))
      server.write(readBuffer, size); // No verification if failed to write to client
  }
  file.close();
}

void HTTPServer::status(WebServer &server, WebServer::ConnectionType type, char *path, bool tail_complete){
  // only GET
  if (type != WebServer::GET){
    server.httpFail();
    return;
  }
  // small buffer 
  if(!tail_complete){
    server.httpFail();
    server.printP(PSTR("Buffer is not big enough."));
    return;
  }
  // Print info
  server.httpSuccess();

  // time / date / uptime
  // temperature indoor
  // temperature outdoor
  // humidity
  // pressure
  // light
  // free memory
  server.printP(PSTR("Free Memory: "));
  server.print(freeMemory());
  server.printP(PSTR(" bytes"));
}

HTTPServer::HTTPServer(){
  if(!webserver)
    webserver=new WebServer("", HTTP_PORT);
  webserver->begin();
  webserver->addCommand("status", &status);
  webserver->addCommand("sd", &fileServer);
  net->addProcessor(loop);
}

uint8_t HTTPServer::loop(){
  char buff[BUFF_TAIL];
  int len;
  len=BUFF_TAIL;
  webserver->processConnection(buff, &len);
  while(webserver->available()){
    len=BUFF_TAIL;
    webserver->processConnection(buff, &len);
  }
  return 0;
}




