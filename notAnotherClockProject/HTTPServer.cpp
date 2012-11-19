#include "HTTPServer.h"
#include <SD.h>
#include <MemoryFree.h>

#define BUFF_READ 64
#define BUFF_TAIL 37
#define BUFF_MIME 26

WebServer webserver("", 80);

void fileServer(WebServer &server, WebServer::ConnectionType type, char *path, bool tail_complete){
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
    server.printP(PSTR("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n<html>\n<title>Arduino File Server</title>\n<body>\n"));
    server.print(path); // TODO add .. link
    server.printP(PSTR("<br/>\n"));
    file.rewindDirectory();
    while(File entry=file.openNextFile()){
      server.printP(PSTR("<a href=\"sd?"));
      server.print(path);
      if(path[strlen(path)-1]!='/')
        server.printP(PSTR("/"));
      server.print(entry.name());
      server.printP(PSTR("\">"));
      server.print(entry.name());
      if(entry.isDirectory())
        server.printP(PSTR("/"));
      server.printP(PSTR("</a><br/>\n"));
      entry.close();
    }
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

void status(WebServer &server, WebServer::ConnectionType type, char *path, bool tail_complete){

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

void HTTPServer::begin(){
  webserver.begin();
  webserver.addCommand("status", &status);
  webserver.addCommand("sd", &fileServer);
}

void HTTPServer::loop(){
  char buff[BUFF_TAIL];
  int len;

  len=BUFF_TAIL;
  webserver.processConnection(buff, &len);
  while(webserver.available()){
    len=BUFF_TAIL;
    webserver.processConnection(buff, &len);
  }
}
