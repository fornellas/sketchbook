#include "HTTPServer.h"

#include <SD.h>

#define FILE_READ_BUFFER 512

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
    server.printP(PSTR("Missing path."));
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
    char buffer[FILE_READ_BUFFER];
    int size;
    server.httpSuccess("text/plain; charset=utf-8", NULL); // fixme, use PSTR
    while(size=file.read(buffer, FILE_READ_BUFFER))
      server.write(buffer, size);
  }
  file.close();
}

void HTTPServer::begin(){
  webserver.begin();
  webserver.addCommand("sd", &fileServer);
}

void HTTPServer::loop(){
  webserver.processConnection();
  while(webserver.available())
    webserver.processConnection();
}
