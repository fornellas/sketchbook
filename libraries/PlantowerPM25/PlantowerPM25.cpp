#include <PlantowerPM25.h>
#include <SoftwareSerial.h>

//
// Definitions
//

#define START_CHAR_1 0x42
#define START_CHAR_2 0x4d
// Datasheet does not specify timing, taking a blind guess here
#define DELAY_RESET_LOW_MS 5
// Datasheet does not specify timing, empirical tests shown we have to wait ~1.5s
#define DELAY_RESET_AFTER_MS 1500
// Datasheet does not specify timing, empirical tests shown the device takes
// ~2.7s to start sending active measurements
#define BOOT_TIME_MS 2800
#define READ_TIMEOUT_MS 2300
// Datasheet does not specify timing, device seems to take some time before
// it can accept new commands
#define DELAY_AFTER_RESPONSE_MS 50

enum command {
  COMMAND_READ_IN_PASSIVE_MODE = 0xe2,
  COMMAND_CHANGE_MODE = 0xe1,
  COMMAND_SLEEP_SET = 0xe4,
};

static const char *error_strings[] = {
  [PLANTOWER_PM25_ERROR_NONE] = "No error",
  [PLANTOWER_PM25_ERROR_UNEXPECTED_RESPONSE] = "Unexpected response",
  [PLANTOWER_PM25_ERROR_SHORT_WRITE] = "Short write",
  [PLANTOWER_PM25_ERROR_INVALID_START_CHAR] = "Invalid start char",
  [PLANTOWER_PM25_ERROR_UNEXPECTED_FRAME_LENGTH] = "Unexpected frame length",
  [PLANTOWER_PM25_ERROR_BAD_CHECKSUM] = "Bad checksum",
  [PLANTOWER_PM25_ERROR_READ_TIMEOUT] = "Read timeout",
};

//
// Structs
//

__attribute__((packed)) struct plantower_pm25_response {
  uint8_t command;
  uint8_t data;
};

__attribute__((packed)) struct plantower_pm25_raw_measurement {
  uint8_t pm1_0_cf1_high;
  uint8_t pm1_0_cf1_low;
  uint8_t pm2_5_cf1_high;
  uint8_t pm2_5_cf1_low;
  uint8_t pm10_cf1_high;
  uint8_t pm10_cf1_low;
  uint8_t pm1_0_atm_env_high;
  uint8_t pm1_0_atm_env_low;
  uint8_t pm2_5_atm_env_high;
  uint8_t pm2_5_atm_env_low;
  uint8_t pm10_atm_env_high;
  uint8_t pm10_atm_env_low;
  uint8_t gt_0_3um_1dL_high;
  uint8_t gt_0_3um_1dL_low;
  uint8_t gt_0_5um_1dL_high;
  uint8_t gt_0_5um_1dL_low;
  uint8_t gt_1_0um_1dL_high;
  uint8_t gt_1_0um_1dL_low;
  uint8_t gt_2_5um_1dL_high;
  uint8_t gt_2_5um_1dL_low;
  uint8_t gt_5_0um_1dL_high;
  uint8_t gt_5_0um_1dL_low;
  uint8_t gt_10um_1dL_high;
  uint8_t gt_10um_1dL_low;
  uint8_t reserved_high;
  uint8_t reserved_low;
};

//
// Helper Functions
//


PlantowerPM25_Error PlantowerPM25::_send_command(uint8_t command, uint16_t data) {
  uint16_t checksum;

  checksum = 0;

  if(_serial->write(START_CHAR_1) != 1)
    return PLANTOWER_PM25_ERROR_SHORT_WRITE;
  checksum += START_CHAR_1;
  if(_serial->write(START_CHAR_2) != 1)
    return PLANTOWER_PM25_ERROR_SHORT_WRITE;
  checksum += START_CHAR_2;

  if(_serial->write(command) != 1)
    return PLANTOWER_PM25_ERROR_SHORT_WRITE;
  checksum += command;

  if(_serial->write(data >> 8) != 1)
    return PLANTOWER_PM25_ERROR_SHORT_WRITE;
  checksum += data >> 8;

  if(_serial->write(data & 0xff) != 1)
    return PLANTOWER_PM25_ERROR_SHORT_WRITE;
  checksum += data & 0xff;

  if(_serial->write(checksum >> 8) != 1)
    return PLANTOWER_PM25_ERROR_SHORT_WRITE;

  if(_serial->write(checksum & 0xff) != 1)
    return PLANTOWER_PM25_ERROR_SHORT_WRITE;

  _serial->flush();

  return PLANTOWER_PM25_ERROR_NONE;
};

PlantowerPM25_Error PlantowerPM25::_wait_for_data(void) {
  unsigned long start_ms;

  start_ms = millis();
  while(!_serial->available()) {
    if(millis() - start_ms > READ_TIMEOUT_MS)
      return PLANTOWER_PM25_ERROR_READ_TIMEOUT;
  }
  return PLANTOWER_PM25_ERROR_NONE;
};

PlantowerPM25_Error PlantowerPM25::_read_packet(uint8_t *data, uint8_t len) {
  uint8_t c;
  uint16_t checksum, frame_checksum;
  uint16_t frame_length;
  enum PlantowerPM25_Error error;

  checksum = 0;

  // Start chars
  if((error = _wait_for_data()))
    return error;
  c = _serial->read();
  if(c != START_CHAR_1)
    return PLANTOWER_PM25_ERROR_INVALID_START_CHAR;
  checksum += c;
  if((error = _wait_for_data()))
    return error;
  c = _serial->read();
  if(c != START_CHAR_2)
    return PLANTOWER_PM25_ERROR_INVALID_START_CHAR;
  checksum += c;

  // Frame Length
  if((error = _wait_for_data()))
    return error;
  c = _serial->read();
  frame_length = ((uint16_t)c) << 8;
  checksum += c;
  if((error = _wait_for_data()))
    return error;
  c = _serial->read();
  frame_length |= c;
  checksum += c;
  if(frame_length != len + 2)
    return PLANTOWER_PM25_ERROR_UNEXPECTED_FRAME_LENGTH;

  // Data
  for(int i=0 ; i < len ; i++) {
    if((error = _wait_for_data()))
      return error;
    c = _serial->read();
    *(data + i) = c;
    checksum += c;
  }

  // Checksum
  if((error = _wait_for_data()))
    return error;
  c = _serial->read();
  frame_checksum = ((uint16_t)c)<<8;
  if((error = _wait_for_data()))
    return error;
  c = _serial->read();
  frame_checksum |= c;
  if(checksum != frame_checksum)
    return PLANTOWER_PM25_ERROR_BAD_CHECKSUM;

  delay(DELAY_AFTER_RESPONSE_MS);

  return PLANTOWER_PM25_ERROR_NONE;
};

PlantowerPM25_Error PlantowerPM25::_setModeNoSync(enum PlantowerPM25_Mode mode) {
  PlantowerPM25_Error error;
  struct plantower_pm25_response response;

  if((error = _send_command(COMMAND_CHANGE_MODE, mode)))
    return error;

  if(error = _read_packet((uint8_t *)&response, sizeof(struct plantower_pm25_response)))
    return error;

  if(response.command != COMMAND_CHANGE_MODE)
    return PLANTOWER_PM25_ERROR_UNEXPECTED_RESPONSE;

  if(response.data != mode)
    return PLANTOWER_PM25_ERROR_UNEXPECTED_RESPONSE;

  _mode = mode;

  return PLANTOWER_PM25_ERROR_NONE;
};

PlantowerPM25_Error PlantowerPM25::_setSleepCommand(bool sleep) {
  struct plantower_pm25_response response;
  enum PlantowerPM25_Error error;
  uint8_t sleep_mode;

  if(sleep)
    sleep_mode = 0;
  else
    sleep_mode = 1;

  if((error = _send_command(COMMAND_SLEEP_SET, sleep_mode)))
      return error;

  if(sleep) {
    if(error = _read_packet((uint8_t *)&response, sizeof(struct plantower_pm25_response)))
      return error;

    if(response.command != COMMAND_SLEEP_SET)
      return PLANTOWER_PM25_ERROR_UNEXPECTED_RESPONSE;

    if(response.data != sleep_mode)
      return PLANTOWER_PM25_ERROR_UNEXPECTED_RESPONSE;
  }

  if(!sleep)
    delay(DELAY_RESET_AFTER_MS);

  return PLANTOWER_PM25_ERROR_NONE;
};

PlantowerPM25_Error PlantowerPM25::begin(Stream *stream) {
  _serial = stream;
  if(_set_pin != 255) {
    pinMode(_set_pin, OUTPUT);
    digitalWrite(_set_pin, HIGH);
  }

  // If we have a reset pin, we can be sure device is in active mode after boot
  if(_reset_pin != 255) {
    pinMode(_reset_pin, OUTPUT);
    digitalWrite(_reset_pin, LOW);
    delay(DELAY_RESET_LOW_MS);
    digitalWrite(_reset_pin, HIGH);
    delay(DELAY_RESET_AFTER_MS);
  // Without a reset pin...
  } else {
    enum PlantowerPM25_Error error;

    // Without a reset pin, sensor may be sleeping so we need to wake it up.
    // The sensor may also be in a half received command state, so we must
    // send wake up twice to ensure it gets back to a consistent state.
    // Sending a wakeup also seems to set the device to active.
    for(uint8_t i=0 ; i < 2 ; i++) {
      error = _setSleepCommand(false);
    }
    if(error)
      return error;
  }

  _mode = PLANTOWER_PM25_MODE_ACTIVE;

  return PLANTOWER_PM25_ERROR_NONE;
};

//
// Public
//

PlantowerPM25_Error PlantowerPM25::begin(HardwareSerial *hardwareSerial, uint8_t set_pin, uint8_t reset_pin) {
  hardwareSerial->begin(9600, SERIAL_8N1);
  _set_pin = set_pin;
  _reset_pin = reset_pin;
  return begin(hardwareSerial);
};

PlantowerPM25_Error PlantowerPM25::begin(HardwareSerial *serial) {
  return begin(serial, -1, -1);
};

PlantowerPM25_Error PlantowerPM25::begin(uint8_t pin_rx, uint8_t pin_tx, uint8_t set_pin, uint8_t reset_pin) {
  SoftwareSerial *softwareSerial = new SoftwareSerial();
  softwareSerial->begin(9600, SWSERIAL_8N1, pin_rx, pin_tx, false);
  _set_pin = set_pin;
  _reset_pin = reset_pin;
  return begin(softwareSerial);
};

PlantowerPM25_Error PlantowerPM25::begin(uint8_t pin_rx, uint8_t pin_tx) {
  return begin(pin_rx, pin_tx, -1, -1);
};

PlantowerPM25_Error PlantowerPM25::setMode(enum PlantowerPM25_Mode mode) {
  PlantowerPM25_Error error;

  if(_mode == PLANTOWER_PM25_MODE_ACTIVE) {
    struct PlantowerPM25_Measurement measurement;

    if(error = readActive(&measurement))
      return error;
  }

  return _setModeNoSync(mode);
};

PlantowerPM25_Error PlantowerPM25::readActive(struct PlantowerPM25_Measurement *measurement) {
  int ret;
  struct plantower_pm25_raw_measurement raw_measurement;
  enum PlantowerPM25_Error error;

  while(error = _read_packet((uint8_t *)&raw_measurement, sizeof(struct plantower_pm25_raw_measurement))) {
    if(error == PLANTOWER_PM25_ERROR_INVALID_START_CHAR)
      continue;
    return error;
  }

  measurement->pm1_0_cf1 = (uint16_t)raw_measurement.pm1_0_cf1_high << 8 | raw_measurement.pm1_0_cf1_low;
  measurement->pm2_5_cf1 = (uint16_t)raw_measurement.pm2_5_cf1_high << 8 | raw_measurement.pm2_5_cf1_low;
  measurement->pm10_cf1 = (uint16_t)raw_measurement.pm10_cf1_high << 8 | raw_measurement.pm10_cf1_low;
  measurement->pm1_0_atm_env = (uint16_t)raw_measurement.pm1_0_atm_env_high << 8 | raw_measurement.pm1_0_atm_env_low;
  measurement->pm2_5_atm_env = (uint16_t)raw_measurement.pm2_5_atm_env_high << 8 | raw_measurement.pm2_5_atm_env_low;
  measurement->pm10_atm_env = (uint16_t)raw_measurement.pm10_atm_env_high << 8 | raw_measurement.pm10_atm_env_low;
  measurement->gt_0_3um_1dL = (uint16_t)raw_measurement.gt_0_3um_1dL_high << 8 | raw_measurement.gt_0_3um_1dL_low;
  measurement->gt_0_5um_1dL = (uint16_t)raw_measurement.gt_0_5um_1dL_high << 8 | raw_measurement.gt_0_5um_1dL_low;
  measurement->gt_1_0um_1dL = (uint16_t)raw_measurement.gt_1_0um_1dL_high << 8 | raw_measurement.gt_1_0um_1dL_low;
  measurement->gt_2_5um_1dL = (uint16_t)raw_measurement.gt_2_5um_1dL_high << 8 | raw_measurement.gt_2_5um_1dL_low;
  measurement->gt_5_0um_1dL = (uint16_t)raw_measurement.gt_5_0um_1dL_high << 8 | raw_measurement.gt_5_0um_1dL_low;
  measurement->gt_10um_1dL = (uint16_t)raw_measurement.gt_10um_1dL_high << 8 | raw_measurement.gt_10um_1dL_low;

  return PLANTOWER_PM25_ERROR_NONE;
};

PlantowerPM25_Error PlantowerPM25::readPassive(struct PlantowerPM25_Measurement *measurement) {
  int ret;
  struct plantower_pm25_raw_measurement raw_measurement;
  enum PlantowerPM25_Error error;

  if((error = _send_command(COMMAND_READ_IN_PASSIVE_MODE, 0)))
    return error;

  return readActive(measurement);
};

PlantowerPM25_Error PlantowerPM25::setSleep(bool sleep) {
  if(_set_pin != 255) {
    if(sleep)
      digitalWrite(_set_pin, LOW);
    else {
      digitalWrite(_set_pin, HIGH);
      delay(DELAY_RESET_AFTER_MS);
    }
  } else {
    if(_mode == PLANTOWER_PM25_MODE_ACTIVE) {
      struct PlantowerPM25_Measurement measurement;
      enum PlantowerPM25_Error error;

      if(error = readActive(&measurement))
        return error;
    }
    _setSleepCommand(sleep);
  }

  return PLANTOWER_PM25_ERROR_NONE;
};

const char *PlantowerPM25::strerror(enum PlantowerPM25_Error errno) {
  return error_strings[errno];
};