#include <MH-Z19C.h>


//
// Definitions
//

#define START_BYTE 0xff
#define RESERVED_BYTE 0x01

static const char *error_strings[] = {
  [MH_Z19C_ERROR_NONE] = "No error",
  [MH_Z19C_ERROR_WRITE_SERIAL] = "Error writing serial data",
  [MH_Z19C_ERROR_INVALID_START_BYTE] = "Invalid start character",
  [MH_Z19C_ERROR_BAD_CHECKSUM] = "Bad checksum",
  [MH_Z19C_ERROR_READ_SERIAL] = "Error reading serial data",
  [MH_Z19C_ERROR_INVALID_RESPONSE_COMMAND] = "Invalid response command",
  [MH_Z19C_ERROR_UNEXPECTED_RESPONSE] = "Unexpected response",
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

enum mh_z19c_error MH_Z19C::_send_command(
  enum _mh_z19c_commands command,
  uint8_t data[5]
) {
  struct mh_z19c_command packet;
  uint8_t *ptr;

  packet.start_byte = START_BYTE;
  packet.reserved = RESERVED_BYTE;
  packet.command = command;
  for(uint8_t i=0 ; i < sizeof(packet.data) ; i++)
    packet.data[i] = data[i];
  packet.checksum = _checksum((uint8_t *)&packet);

  ptr=(uint8_t *)(&packet);
  for(uint8_t i=0 ; i < sizeof(struct mh_z19c_command) ; i++, ptr++)
    if(_serial->write(*ptr) != 1)
      return MH_Z19C_ERROR_WRITE_SERIAL;
  _serial->flush();

  return MH_Z19C_ERROR_NONE;
}

enum mh_z19c_error MH_Z19C::_read_return_value(
  struct mh_z19c_return_value *return_value
) {
  uint8_t c;

  // Start byte
  while(!_serial->available());
  c = _serial->read();
  if(c != START_BYTE)
    return MH_Z19C_ERROR_INVALID_START_BYTE;
  return_value->start_byte = c;

  // Command
  while(!_serial->available());
  c = _serial->read();
  return_value->command = c;

  // Data
  for(uint8_t i=0; i < sizeof(return_value->data) ; i++){
    while(!_serial->available());
    return_value->data[i] = _serial->read();
  }

  // Checksum
  while(!_serial->available());
  c = _serial->read();
  return_value->checksum = c;
  if(return_value->checksum != _checksum((uint8_t *)return_value))
    return MH_Z19C_ERROR_BAD_CHECKSUM;

  return MH_Z19C_ERROR_NONE;
}

//
// Public
//

void MH_Z19C::begin(uint8_t pin_hd, HardwareSerial *serial) {
  _pin_hd = pin_hd;
  pinMode(_pin_hd, OUTPUT);
  digitalWrite(_pin_hd, HIGH);

  _serial = serial;
  _serial->begin(9600, SERIAL_8N1);

  // Sensor often takes ~2min to be able to receive requests
  delay(180*1000);
}

mh_z19c_error MH_Z19C::read(uint16_t *co2_concentration) {
  enum mh_z19c_error ret;
  uint8_t data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
  struct mh_z19c_return_value return_value;

  if((ret=_send_command(COMMAND_READ_CO2_CONCENTRATION, data)))
    return ret;

  if((ret=_read_return_value(&return_value)))
    return ret;

  if((return_value.command != COMMAND_READ_CO2_CONCENTRATION))
    return MH_Z19C_ERROR_INVALID_RESPONSE_COMMAND;

  *co2_concentration = (uint16_t)(return_value.data[0]) * 256 + return_value.data[1];

  return MH_Z19C_ERROR_NONE;
}

enum mh_z19c_error MH_Z19C::selfCalibrationForZeroPoint(
  bool enabled
) {
  uint8_t data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
  struct mh_z19c_return_value return_value;
  enum mh_z19c_error ret;

  if(enabled)
    data[0] = 0xa0;

  if((ret=_send_command(COMMAND_SELF_CALIBRATION_FUNCTION_FOR_ZERO_POINT, data)))
    return ret;

  if((ret=_read_return_value(&return_value)))
    return ret;

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

const char *MH_Z19C::strerror(enum mh_z19c_error mh_z19c_errno) {
  return error_strings[mh_z19c_errno];
}