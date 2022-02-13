#ifndef PlantowerPM25_h
#define PlantowerPM25_h

#include "Arduino.h"
#include <stdint.h>

// Same protocol
// PMS7003
// PMS6003
// PMS5003
// PMS1003
// Different / inconsistent datasheet
// PMS3003


enum PlantowerPM25_Error {
  PLANTOWER_PM25_ERROR_NONE,
  PLANTOWER_PM25_ERROR_UNEXPECTED_RESPONSE,
  PLANTOWER_PM25_ERROR_SHORT_WRITE,
  PLANTOWER_PM25_ERROR_INVALID_START_CHAR,
  PLANTOWER_PM25_ERROR_UNEXPECTED_FRAME_LENGTH,
  PLANTOWER_PM25_ERROR_BAD_CHECKSUM,
  PLANTOWER_PM25_ERROR_READ_TIMEOUT,
};

struct PlantowerPM25_Measurement {
  // PM1.0 concentration unit μg/m³ (CF=1，standard particle)
  uint16_t pm1_0_cf1;
  // PM2.5 concentration unit μg/m³ (CF=1，standard particle)
  uint16_t pm2_5_cf1;
  // PM10 concentration unit μg/m³ (CF=1，standard particle)
  uint16_t pm10_cf1;

  // PM1.0 concentration unit μg/m³ (under atmospheric environment)
  uint16_t pm1_0_atm_env;
  //  PM2.5 concentration unit μg/m³ (under atmospheric environment)
  uint16_t pm2_5_atm_env;
  // Concentration unit (under atmospheric environment) μg/m³
  uint16_t pm10_atm_env;

  // Number of particles with diameter beyond 0.3µm in 0.1L of air.
  uint16_t gt_0_3um_1dL;
  // Number of particles with diameter beyond 0.5µm in 0.1L of air.
  uint16_t gt_0_5um_1dL;
  // Number of particles with diameter beyond 1.0µm in 0.1L of air.
  uint16_t gt_1_0um_1dL;
  // Number of particles with diameter beyond 2.5µm in 0.1L of air.
  uint16_t gt_2_5um_1dL;
  // Number of particles with diameter beyond 5.0µm in 0.1L of air.
  uint16_t gt_5_0um_1dL;
  // Number of particles with diameter beyond 10µm in 0.1L of air.
  uint16_t gt_10um_1dL;
};

enum PlantowerPM25_Mode {
  PLANTOWER_PM25_MODE_PASSIVE,
  PLANTOWER_PM25_MODE_ACTIVE,
};

class PlantowerPM25 {
  public:
  
  PlantowerPM25_Error begin(HardwareSerial *serial, uint8_t set_pin, uint8_t reset_pin);
  PlantowerPM25_Error begin(HardwareSerial *serial);
  PlantowerPM25_Error begin(uint8_t pin_rx, uint8_t pin_tx, uint8_t set_pin, uint8_t reset_pin);
  PlantowerPM25_Error begin(uint8_t pin_rx, uint8_t pin_tx);

  PlantowerPM25_Error setMode(enum PlantowerPM25_Mode mode);

  PlantowerPM25_Error readActive(struct PlantowerPM25_Measurement *measurement);
  PlantowerPM25_Error readPassive(struct PlantowerPM25_Measurement *measurement);

  PlantowerPM25_Error setSleep(bool sleep);

  const char *strerror(enum PlantowerPM25_Error errno);
  
  private:

  PlantowerPM25_Error begin(Stream *stream);

  Stream* _serial;
  uint8_t _reset_pin = -1;
  uint8_t _set_pin = -1;
  enum PlantowerPM25_Mode _mode;

  PlantowerPM25_Error _send_command(uint8_t command, uint16_t data);
  PlantowerPM25_Error _wait_for_data(void);
  PlantowerPM25_Error _read_packet(uint8_t *data, uint8_t len);
  PlantowerPM25_Error _setModeNoSync(enum PlantowerPM25_Mode mode);
  PlantowerPM25_Error _setSleepCommand(bool sleep);
};

class PMS7003 : public PlantowerPM25 {};

class PMS6003 : public PlantowerPM25 {};

class PMS5003 : public PlantowerPM25 {};

// Datasheet is a bit inconsistent:
// - It says the data bytes 7, 8 and 9 for "0.1 L of air" are "reserved".
// - It does not not show data bytes 10, 11, 12 and 13.
// - "Data and check" bytes refer to 13 total data bytes.
// class PMS3003 : public PlantowerPM25 {};

class PMS1003 : public PlantowerPM25 {};

#endif