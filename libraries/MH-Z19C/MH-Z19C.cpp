#include <MH-Z19C.h>
#include <SoftwareSerial.h>

//
// Definitions
//

// Datasheet mentions < 1min, but empirical testing has shown that the sensor
// after power on:
// - Immediately spits some "stuff" to serial.
// - Before ~15s, if queried, it returns "garbage" back.
// - Between ~15s and ~75s returns 500 or 505 CO2 ppm.
// - After ~75s it finally returns valid readings.
// #define WARMUP_SECONDS 76
#define WARMUP_SECONDS 15
#define START_BYTE 0xff
#define RESERVED_BYTE 0x01
#define READ_TIMEOUT_MS 50

static const char *error_strings[] = {
  [MH_Z19C_ERROR_NONE] = "No error",
  [MH_Z19C_ERROR_WARMING_UP] = "Sensor is warming up",
  [MH_Z19C_ERROR_WRITE_SERIAL] = "Error writing serial data",
  [MH_Z19C_ERROR_INVALID_START_BYTE] = "Invalid start character",
  [MH_Z19C_ERROR_BAD_CHECKSUM] = "Bad checksum",
  [MH_Z19C_ERROR_READ_SERIAL] = "Error reading serial data",
  [MH_Z19C_ERROR_INVALID_RESPONSE_COMMAND] = "Invalid response command",
  [MH_Z19C_ERROR_UNEXPECTED_RESPONSE] = "Unexpected response",
  [MH_Z19C_ERROR_READ_TIMEOUT] = "Read timeout.",
};

//
// Structs
//

struct __attribute__((packed)) mh_z19c_command {
  uint8_t start_byte;
  uint8_t reserved;
  uint8_t command;
  uint8_t data[5];
  uint8_t checksum;
};

struct __attribute__((packed)) mh_z19c_return_value {
  uint8_t start_byte;
  uint8_t command;
  uint8_t data[6];
  uint8_t checksum;
};

//
// Helper Functions
//

uint8_t MH_Z19C::_checksum(uint8_t *packet) {
  uint8_t value;

  value = 0;
  packet++;

  for(int i=0 ; i < 7 ; i++, packet++){
    value += *packet;
  }

  value = 0xff - value;
  value++;

  return value;
}

enum MH_Z19C_Error MH_Z19C::_send_command(
  enum _mh_z19c_commands command,
  uint8_t data[5]
) {
  struct mh_z19c_command packet;
  uint8_t *ptr;

  if(((millis() - _begin_time) / 1000) < WARMUP_SECONDS)
    return MH_Z19C_ERROR_WARMING_UP;

  packet.start_byte = START_BYTE;
  packet.reserved = RESERVED_BYTE;
  packet.command = command;
  for(uint8_t i=0 ; i < sizeof(packet.data) ; i++)
    packet.data[i] = data[i];
  packet.checksum = _checksum((uint8_t *)&packet);

  ptr=(uint8_t *)(&packet);
  for(uint8_t i=0 ; i < sizeof(struct mh_z19c_command) ; i++, ptr++) {
    if(_serial->write(*ptr) != 1)
      return MH_Z19C_ERROR_WRITE_SERIAL;
  }
  _serial->flush();

  return MH_Z19C_ERROR_NONE;
}

enum MH_Z19C_Error MH_Z19C::_wait_for_data(void) {
  unsigned long start_ms;

  start_ms = millis();
  while(!_serial->available()) {
    if(millis() - start_ms > READ_TIMEOUT_MS)
      return MH_Z19C_ERROR_READ_TIMEOUT;
  }
  return MH_Z19C_ERROR_NONE;
}

enum MH_Z19C_Error MH_Z19C::_read_return_value(
  struct mh_z19c_return_value *return_value
) {
  uint8_t c;
  enum MH_Z19C_Error err;

  // Start byte
  err = _wait_for_data();
  if(err != MH_Z19C_ERROR_NONE)
    return err;
  c = _serial->read();
  if(c != START_BYTE)
    return MH_Z19C_ERROR_INVALID_START_BYTE;
  return_value->start_byte = c;

  // Command
  err = _wait_for_data();
  if(err != MH_Z19C_ERROR_NONE)
    return err;
  c = _serial->read();
  return_value->command = c;

  // Data
  for(uint8_t i=0; i < sizeof(return_value->data) ; i++){
    err = _wait_for_data();
    if(err != MH_Z19C_ERROR_NONE)
      return err;
    return_value->data[i] = _serial->read();
  }

  // Checksum
  err = _wait_for_data();
  if(err != MH_Z19C_ERROR_NONE)
    return err;
  c = _serial->read();
  return_value->checksum = c;
  if(return_value->checksum != _checksum((uint8_t *)return_value))
    return MH_Z19C_ERROR_BAD_CHECKSUM;

  return MH_Z19C_ERROR_NONE;
}

void MH_Z19C::begin(Stream *stream, uint8_t pin_hd) {
  _begin_time = millis();

  _serial = stream;

  _pin_hd = pin_hd;
  pinMode(_pin_hd, OUTPUT);
  digitalWrite(_pin_hd, HIGH);
}

//
// Public
//

void MH_Z19C::begin(HardwareSerial *serial, uint8_t pin_hd) {
  serial->begin(9600, SERIAL_8N1);
  begin(serial, pin_hd);
}

void MH_Z19C::begin(uint8_t pin_rx, uint8_t pin_tx, uint8_t pin_hd) {
  SoftwareSerial *serial = new SoftwareSerial();
  serial->begin(9600, SWSERIAL_8N1, pin_rx, pin_tx, false);
  begin(serial, pin_hd);
}

MH_Z19C_Error MH_Z19C::read(uint16_t *co2_concentration) {
  enum MH_Z19C_Error err;
  uint8_t data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
  struct mh_z19c_return_value return_value;

  if((err=_send_command(COMMAND_READ_CO2_CONCENTRATION, data)))
    return err;

  if((err=_read_return_value(&return_value)))
    return err;

  if((return_value.command != COMMAND_READ_CO2_CONCENTRATION))
    return MH_Z19C_ERROR_INVALID_RESPONSE_COMMAND;

  *co2_concentration = (uint16_t)(return_value.data[0]) * 256 + return_value.data[1];

  return MH_Z19C_ERROR_NONE;
}

enum MH_Z19C_Error MH_Z19C::selfCalibrationForZeroPoint(bool enabled ) {
  uint8_t data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
  struct mh_z19c_return_value return_value;
  enum MH_Z19C_Error err;

  if(enabled)
    data[0] = 0xa0;

  if((err=_send_command(COMMAND_SELF_CALIBRATION_FUNCTION_FOR_ZERO_POINT, data)))
    return err;

  if((err=_read_return_value(&return_value)))
    return err;

  if(return_value.command != COMMAND_SELF_CALIBRATION_FUNCTION_FOR_ZERO_POINT)
    return MH_Z19C_ERROR_INVALID_RESPONSE_COMMAND;

  if(return_value.data[0] != 1)
    return MH_Z19C_ERROR_UNEXPECTED_RESPONSE;

  for(uint8_t i=1 ; i < sizeof(return_value.data) ; i++)
    if(return_value.data[i] != 0)
      return MH_Z19C_ERROR_UNEXPECTED_RESPONSE;

  return MH_Z19C_ERROR_NONE;
}

void MH_Z19C::zeroPointCalibration(void) {
  digitalWrite(_pin_hd, LOW);
  delay(8*1000);
  digitalWrite(_pin_hd, HIGH);
}

const char *MH_Z19C::strerror(enum MH_Z19C_Error mh_z19c_errno) {
  return error_strings[mh_z19c_errno];
}