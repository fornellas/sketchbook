#include <SetupPortal.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define MIN(a,b) (((a)<(b))?(a):(b))

#define SSID_SUFFIX_LENGTH 7
#define MAX_SSID_PREFIX_LENGTH (WL_SSID_MAX_LENGTH - SSID_SUFFIX_LENGTH)
#define MAX_NAME_LENGTH MIN(MAX_SSID_PREFIX_LENGTH, MDNS_DOMAIN_LABEL_MAXLENGTH)
#define RESTART_DELAY 2000

SetupPortalClass SetupPortal;
ESP8266WebServer webServer(80);

void SetupPortalClass::_loadHostname() {
  bool hasNull = false;
  for(int i=0;i<MDNS_DOMAIN_LABEL_MAXLENGTH;i++) {
    hostname[i] = EEPROM.read(_eepromAddressHostname + i);
    if(hostname[i] == '\0')
      hasNull = true;
  }
  if(!hasNull)
    hostname[0] = '\0';
  if(!strlen(hostname)) {
    _saveHostname(_name);
  }
}

bool SetupPortalClass::_saveHostname(const char *newHostname) {
  if(strlen(newHostname) > MDNS_DOMAIN_LABEL_MAXLENGTH)
    return false;
  for(int i=0;i<MDNS_DOMAIN_LABEL_MAXLENGTH;i++) {
    EEPROM.write(_eepromAddressHostname + i, newHostname[i]);
  }
  EEPROM.commit();
  _loadHostname();
  return true;
}

bool SetupPortalClass::begin(const char *name, int eepromAddressHostname) {
  if(String(name).length() > MAX_NAME_LENGTH)
    return false;
  _name = name;
  _eepromAddressHostname = eepromAddressHostname;
  _loadHostname();
  _restart = false;
  return true;
};

bool SetupPortalClass::_setupWifi(IPAddress *localIP) {
  // Wifi AP
  *localIP = IPAddress(10, random(0, 255), random(0, 255), 1);
  uint8_t mac[6];
  WiFi.softAPmacAddress(mac);
  char mac_str[SSID_SUFFIX_LENGTH + 1];
  sprintf(mac_str, "-%02x%02x%02x", mac[3], mac[4], mac[5]);
  String ap_ssid = String(_name) + String(mac_str);
  if(!WiFi.mode(WIFI_AP_STA))
    return false;
  if(!WiFi.softAPConfig(*localIP, *localIP, IPAddress(255, 255, 255, 0)))
    return false;
  if(!WiFi.softAP(ap_ssid))
    return false;

  // Wifi ST
  WiFi.persistent(true);
  WiFi.begin();

  return true;
};

bool SetupPortalClass::_setupWebServer() {
  webServer.on(
    "/", HTTP_GET, [&]() {
      // TODO set no cache
      if(!webServer.chunkedResponseModeStart(200,"text/html")) {
        webServer.send(505, "text/plain", "HTTP1.1 required");
        return;
      }
      webServer.sendContent(
        "<!DOCTYPE html>"
        "<html>"
        "  <head>"
        "    <title>Setup</title>"
        "  </head>"
        "  <h1>Setup</h1>"
        "  <h2>1. Wifi</h2>"
        "  <h3>Status</h3>"
        "  <form method=\"get\">"
        "    <label>SSID: "
      );
      String ssid;
      ssid = WiFi.SSID();
      if(ssid.length())
        webServer.sendContent(ssid);
      else
        webServer.sendContent("(unset)");
      webServer.sendContent(
        "</label><br>"
        "    <label>Status: "
      );
      String status;
      switch(WiFi.status()) {
        case WL_NO_SHIELD:
          status = "No shield";
          break;
        case WL_IDLE_STATUS:
          status = "Idle";
          break;
        case WL_NO_SSID_AVAIL:
          status = "No SSID available";
          break;
        case WL_SCAN_COMPLETED:
          status = "Scan completed";
          break;
        case WL_CONNECTED:
          status = "Connected";
          break;
        case WL_CONNECT_FAILED:
          status = "Connect failed";
          break;
        case WL_CONNECTION_LOST:
          status = "Connection lost";
          break;
        case WL_WRONG_PASSWORD:
          status = "Wrong password";
          break;
        case WL_DISCONNECTED:
          status = "Disconnected";
          break;
        default:
          status = "Unknown";
          break;
      }
      webServer.sendContent(status);
      webServer.sendContent(
        "</label><br>"
        "    <input type=\"submit\" value=\"Refresh\">"
        "  </form>"
        "  <h3>Credentials</h3>"
        "  <form method=\"post\" action=\"/wifi/save\">"
        "    <label>SSID: <input type=\"text\" name=\"ssid\" value=\""
      );
      if(ssid.length()) {
        webServer.sendContent(ssid);
      }
      webServer.sendContent(
        "\"></label><br>"
        "    <label>Password: <input type=\"password\" name=\"psk\"></label><br>"
        "    <input type=\"submit\" value=\"Save\">"
        "  </form>"
        "  <form method=\"post\" action=\"/wifi/clear\">"
        "    <input type=\"submit\" value=\"Clear\">"
        "  </form>"
        "  <h2>2. mDNS</h2>"
        "  <form method=\"post\" action=\"/mdns/save\">"
        "    <label>Hostname: <input type=\"text\" name=\"hostname\" value=\""
      );
      if(strlen(hostname))
        webServer.sendContent(hostname);
      webServer.sendContent(
        "\"></label>.local<br>"
        "    <input type=\"submit\" value=\"Save\">"
        "  </form>"
        "  <form method=\"post\" action=\"/mdns/reset\">"
        "    <input type=\"submit\" value=\"Reset\">"
        "  </form>"
        "  <h2>3. Finish</h2>"
        "  <form method=\"post\" action=\"/restart\">"
        "    <input type=\"submit\" value=\"Finish\">"
        "  </form>"
        "</html>"
      );
    }
  );

  webServer.on(
    "/wifi/save", HTTP_POST, [&]() {
      // TODO set no cache
      // FIXME validate limit length
      WiFi.disconnect();
      String ssid = webServer.arg("ssid");
      String psk = webServer.arg("psk");
      WiFi.begin(ssid, psk);

      webServer.sendHeader("Location", "/");
      webServer.send(302);
    }
  );

  webServer.on(
    "/wifi/clear", HTTP_POST, [&]() {
      // TODO set no cache
      WiFi.disconnect();

      webServer.sendHeader("Location", "/");
      webServer.send(302);
    }
  );

  webServer.on(
    "/mdns/save", HTTP_POST, [&]() {
      // TODO set no cache
      // FIXME validate limit length
      String hostname = webServer.arg("hostname");
      _saveHostname(hostname.c_str());

      webServer.sendHeader("Location", "/");
      webServer.send(302);
    }
  );

  webServer.on(
    "/mdns/reset", HTTP_POST, [&]() {
      // TODO set no cache
      _saveHostname(_name);

      webServer.sendHeader("Location", "/");
      webServer.send(302);
    }
  );

  webServer.on(
    "/restart", HTTP_POST, [&]() {
      webServer.send(
        200, "text/html",
        "<!DOCTYPE html>"
        "<html>"
        "  <head>"
        "    <title>Setup</title>"
        "  </head>"
        "  <h1>Setup</h1>"
        "  Setup complete."
        "</html>"
      );
      _restart = true;
      _restartMillis = millis();
    }
  );

  webServer.begin();

  return true;
};

bool SetupPortalClass::runLoop() {
  IPAddress localIP;

  if(!_setupWifi(&localIP))
    return false;

  DNSServer dnsServer;
  if(!dnsServer.start(53, "*", localIP))
    return false;

  if(!_setupWebServer())
    return false;

  if(!MDNS.begin(_name))
    return false;
  MDNS.addService((String("Setup ") + String(_name)).c_str(), "http", "tcp", 80);

  while(true) {
    dnsServer.processNextRequest();
    webServer.handleClient();
    MDNS.update();
    if(_restart && (millis() - _restartMillis > RESTART_DELAY))
        ESP.restart();
  }
};

bool SetupPortalClass::runLoopIfExtSysReset() {
  if(ESP.getResetInfoPtr()->reason != REASON_EXT_SYS_RST)
    return true;
  return runLoop();
};