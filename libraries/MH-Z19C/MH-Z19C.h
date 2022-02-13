#ifndef MH_Z19C_h
#define MH_Z19C_h

#include "Arduino.h"
#include <stdint.h>


enum MH_Z19C_Error {
  MH_Z19C_ERROR_NONE,
  MH_Z19C_ERROR_WARMING_UP,
  MH_Z19C_ERROR_WRITE_SERIAL,
  MH_Z19C_ERROR_INVALID_START_BYTE,
  MH_Z19C_ERROR_BAD_CHECKSUM,
  MH_Z19C_ERROR_READ_SERIAL,
  MH_Z19C_ERROR_INVALID_RESPONSE_COMMAND,
  MH_Z19C_ERROR_UNEXPECTED_RESPONSE,
  MH_Z19C_ERROR_READ_TIMEOUT,
};


class MH_Z19C {
  public:
  
  void begin(HardwareSerial *serial, uint8_t pin_hd);
  void begin(uint8_t pin_rx, uint8_t pin_tx, uint8_t pin_hd);
  MH_Z19C_Error read(uint16_t *co2_concentration);
  MH_Z19C_Error selfCalibrationForZeroPoint(bool enable);
  void zeroPointCalibration(void);
  const char *strerror(enum MH_Z19C_Error mh_z19c_errno);
  
  private:

  uint8_t _pin_hd;
  Stream* _serial;
  unsigned long _begin_time;

  enum _mh_z19c_commands {
    COMMAND_READ_CO2_CONCENTRATION = 0x86,
    COMMAND_SELF_CALIBRATION_FUNCTION_FOR_ZERO_POINT = 0x79,
  };

  void begin(Stream *stream, uint8_t pin_hd);

  uint8_t _checksum(uint8_t *packet);
  enum MH_Z19C_Error _send_command(
    enum _mh_z19c_commands command,
    uint8_t data[5]
  );
  enum MH_Z19C_Error _wait_for_data(void);
  enum MH_Z19C_Error _read_return_value(
    struct mh_z19c_return_value *return_value
  );
};

#endif