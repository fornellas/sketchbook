#include <IPAddress.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

class SetupPortalClass {
  public:
    char hostname[MDNS_DOMAIN_LABEL_MAXLENGTH+1];
    
    bool begin(const char *name, int eepromAddressHostname);
    bool runLoop(void);
    bool runLoopIfExtSysReset();
  private:
    const char *_name;
    int _eepromAddressHostname;
    bool _restart;
    unsigned long _restartMillis;

    void _loadHostname();
    bool _saveHostname(const char *newHostname);
    bool _setupWifi(IPAddress *localIP);
    bool _setupWebServer();
};

extern SetupPortalClass SetupPortal;