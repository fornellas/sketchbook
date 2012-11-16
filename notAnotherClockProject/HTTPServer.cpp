#include "HTTPServer.h"

#include <SD.h>
#include <MemoryFree.h>

#define FILE_READ_BUFFER 64

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
    server.printP(PSTR("Buffer is not big enough."));
    return;
  }
  // no path
  if(!strlen(path)){
    server.httpFail();
    server.printP(PSTR("Missing file path."));
    return;
  }
  // File not found
  if(!(path[0]=='/'&&path[1]=='\0'))
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
    server.print(path);
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
  // dump files
  } else {
    char *readBuffer;
    char *mimeBuffer;
    int size;
    const PROGMEM char *mime=PSTR("text/plain; charset=utf-8");
    if(mimeBuffer=(char *)malloc(strlen_P(mime)+1)){
      strcpy_P(mimeBuffer, mime);
      if(readBuffer=(char *)malloc(FILE_READ_BUFFER)){
        server.httpSuccess(mimeBuffer, NULL);
        while(size=file.read(readBuffer, FILE_READ_BUFFER))
          server.write(readBuffer, size);
        free(readBuffer);
      }else{
        server.httpFail();
        server.printP(PSTR("Unable to allocate reading buffer."));
      }
      free(mimeBuffer);
    }else{
      server.httpFail();
      server.printP(PSTR("Unable to allocate message buffer."));
    }
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
  webserver.addCommand("status", &status); // FIXME save string to Flash
  webserver.addCommand("sd", &fileServer); // FIXME save string to Flash
}

void HTTPServer::loop(){
  webserver.processConnection();
  while(webserver.available())
    webserver.processConnection();
}
