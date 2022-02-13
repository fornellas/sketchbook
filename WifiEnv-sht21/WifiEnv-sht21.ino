#include <SHT21.h>
#include <MH-Z19C.h>
#include <I2CClearBus.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define PIN_MH_Z19C_HD 12

#define PIN_SDA 4  // D2
#define PIN_SCL 5  // D1

#define SSID "VM2B47AA2-2.4GHz"
#define PASSPHRASE "hr8djGtsnQhu"

SHT21 sht21;
MH_Z19C mh_z19c;
ESP8266WebServer prometheusWebServer(9090);

void error(const char *msg) {
  Serial.flush();
  Serial.swap();
  Serial.print("Error: ");
  Serial.println(msg);
  while(true) delay(1000);
}

void getMetrics() {
  enum mh_z19c_error mh_z19c_error;
  SHT21Error sht21_error;
  double temperature, rh;
  uint16_t co2_concentration;
  char const *response_template =
    "temperature_celsius{room=\"office\"} %lf\n"
    "relative_humidity_ratio{room=\"office\"} %lf\n"
    "co2_concentration_ppm{room=\"office\"} %hu\n";
  char response[1024];


  if((sht21_error=sht21.GetTemperature(&temperature))) {
    prometheusWebServer.send(500, "text/plain", "SHT21.GetTemperature");
    return;
  }
  if((sht21_error=sht21.GetRelativeHumidity(&rh))) {
    prometheusWebServer.send(500, "text/plain", "SHT21.GetRelativeHumidity");
    return;
  }

  if((mh_z19c_error = mh_z19c.read(&co2_concentration))) {
    prometheusWebServer.send(500, "text/plain", "MH_Z19C.read");
    return; 
  }

  snprintf(response, 1024, response_template, temperature, rh / 100.0, co2_concentration);
  prometheusWebServer.send(200, "text/plain; charset=utf-8", response);
}

void setup(){
  enum mh_z19c_error mh_z19c_error;
  SHT21Error sht21_error;
  I2CClearBusError i2c_clearbus_error;

  // MH-Z19C

  mh_z19c.begin(PIN_MH_Z19C_HD, &Serial);
  Serial.flush();
  Serial.swap();
  if((mh_z19c_error = mh_z19c.selfCalibrationForZeroPoint(false))) {
    error("MH_Z19C.selfCalibrationForZeroPoint");
  }

  // I2C

  if((i2c_clearbus_error=I2CClearBus(PIN_SDA, PIN_SCL)))
    error("I2CClearBus");
#if defined(ESP8266) || defined(ESP32)
  Wire.begin(PIN_SDA, PIN_SCL);
#else
  Wire.begin();
#endif

  // SHT21

  if((sht21_error=sht21.begin()))
    error("SHT21.begin");

  // Wifi

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSPHRASE);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  // Prometheus Web Server

  prometheusWebServer.on("/metrics", HTTPMethod::HTTP_GET, getMetrics);
  prometheusWebServer.begin();
}

void loop(){
  prometheusWebServer.handleClient();
}
