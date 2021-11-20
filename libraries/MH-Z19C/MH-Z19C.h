#ifndef MH_Z19C_h
#define MH_Z19C_h

#include "Arduino.h"
#include <stdint.h>


enum mh_z19c_error {
  MH_Z19C_ERROR_NONE,
  MH_Z19C_ERROR_WRITE_SERIAL,
  MH_Z19C_ERROR_INVALID_START_BYTE,
  MH_Z19C_ERROR_BAD_CHECKSUM,
  MH_Z19C_ERROR_READ_SERIAL,
  MH_Z19C_ERROR_INVALID_RESPONSE_COMMAND,
  MH_Z19C_ERROR_UNEXPECTED_RESPONSE,
};

enum _mh_z19c_commands {
  COMMAND_READ_CO2_CONCENTRATION = 0x86,
  COMMAND_SELF_CALIBRATION_FUNCTION_FOR_ZERO_POINT = 0x79,
};

class MH_Z19C {
  public:
  
  void begin(uint8_t pin_hd, HardwareSerial *serial = &Serial);
  mh_z19c_error read(uint16_t *co2_concentration);
  mh_z19c_error selfCalibrationForZeroPoint(bool enable);
  void zeroPointCalibration(void);
  const char *strerror(enum mh_z19c_error mh_z19c_errno);
  
  private:

  uint8_t _pin_hd;
  HardwareSerial* _serial;

  uint8_t _checksum(uint8_t *packet);
  enum mh_z19c_error _send_command(
    enum _mh_z19c_commands command,
    uint8_t data[5]
  );
  enum mh_z19c_error _read_return_value(
    struct mh_z19c_return_value *return_value
  );
};

#endif