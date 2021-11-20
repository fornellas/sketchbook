#include <SHT21.h>

#define I2C_7_BIT_ADDRESS 0b1000000
#define SOFT_RESET_MS 15
#define STARTUP_MS 15

enum Commands {
  TRIGGER_T_MEASUREMENT_HOLD_MASTER     = 0b11100011,
  TRIGGER_RH_MEASUREMENT_HOLD_MASTER    = 0b11100101,
  TRIGGER_T_MEASUREMENT_NO_HOLD_MASTER  = 0b11110011,
  TRIGGER_RH_MEASUREMENT_NO_HOLD_MASTER = 0b11110101,
  WRITE_USER_REGISTER                   = 0b11100110,
  READ_USER_REGISTER                    = 0b11100111,
  SOFT_RESET                            = 0b11111110,
};

struct RawMeasurement {
  uint16_t data;
  uint8_t checksum;
};

SHT21Error SHT21::SoftReset() {
  _wire->beginTransmission(I2C_7_BIT_ADDRESS);

  if(_wire->write(SOFT_RESET) != 1)
    return SHT21_ERROR_WIRE_WRITE;

  switch(_wire->endTransmission()) {
    case 0:
      break;
    case 1:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_LENGTH_TO_LONG_FOR_BUFFER;
    case 2:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_ADDRESS_SEND_NACK_RECEIVED;
    case 3:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_DATA_SEND_NACK_RECEIVED;
    case 4:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_OTHER_TWI_ERROR;
    case 5:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_TIMEOUT;
    default:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_UNKNOWN;
  }

  delay(SOFT_RESET_MS);

  return SHT21_OK;
};

SHT21Error SHT21::begin(TwoWire *wire) {
  _wire = wire;
  delay(STARTUP_MS);
  return SoftReset();
};

SHT21Error SHT21::_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum){
  const int16_t POLYNOMIAL = 0x131; //P(x)=x^8+x^5+x^4+1 = 100110001
  uint8_t crc = 0;
  uint8_t byteCtr;

  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr) {
    crc ^= (data[byteCtr]);
    for (uint8_t bit = 8; bit > 0; --bit) {
      if (crc & 0x80)
        crc = (crc << 1) ^ POLYNOMIAL;
      else
        crc = (crc << 1);
    }
  }
  if (crc != checksum)
    return SHT21_ERROR_CHECKSUM;
  else
    return SHT21_OK;
}

SHT21Error SHT21::_ReadRawMeasurement(struct RawMeasurement *raw_measurement) {
  SHT21Error err;

  if(_wire->requestFrom(I2C_7_BIT_ADDRESS, 3) != 3)
    return SHT21_ERROR_WIRE_REQUESTFROM;

  raw_measurement->data = 0;
  raw_measurement->data = (uint16_t)(_wire->read())<<8;
  raw_measurement->data |= (uint16_t)(_wire->read());
  raw_measurement->checksum = _wire->read();

  if((err = _CheckCrc((uint8_t *)(&raw_measurement->data), sizeof(raw_measurement->data), raw_measurement->checksum)))
    return err;

  return SHT21_OK;
}

double SHT21::_ConvertTemperature(uint16_t data) {
   return -46.85 + (175.72 / 65536.0) * (data & 0xFFFC);
};

SHT21Error SHT21::GetTemperature(double *temperature) {
  struct RawMeasurement raw_measurement;
  
  _wire->beginTransmission(I2C_7_BIT_ADDRESS);
  if(_wire->write(TRIGGER_T_MEASUREMENT_HOLD_MASTER) != 1)
    return SHT21_ERROR_WIRE_WRITE;

  switch(_wire->endTransmission(false)) {
    case 0:
      break;
    case 1:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_LENGTH_TO_LONG_FOR_BUFFER;
    case 2:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_ADDRESS_SEND_NACK_RECEIVED;
    case 3:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_DATA_SEND_NACK_RECEIVED;
    case 4:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_OTHER_TWI_ERROR;
    case 5:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_TIMEOUT;
    default:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_UNKNOWN;
  }

  _ReadRawMeasurement(&raw_measurement);

  *temperature = _ConvertTemperature(raw_measurement.data);
  
  return SHT21_OK;
};

double SHT21::_ConvertRelativeHumidity(uint16_t data) {
   return -6.0 + (125.0 / 65536.0) * (data & 0xFFFC);
};

SHT21Error SHT21::GetRelativeHumidity(double *rh) {
  struct RawMeasurement raw_measurement;
  
  _wire->beginTransmission(I2C_7_BIT_ADDRESS);
  if(_wire->write(TRIGGER_RH_MEASUREMENT_HOLD_MASTER) != 1)
    return SHT21_ERROR_WIRE_WRITE;

  switch(_wire->endTransmission(false)) {
    case 0:
      break;
    case 1:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_LENGTH_TO_LONG_FOR_BUFFER;
    case 2:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_ADDRESS_SEND_NACK_RECEIVED;
    case 3:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_DATA_SEND_NACK_RECEIVED;
    case 4:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_OTHER_TWI_ERROR;
    case 5:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_TIMEOUT;
    default:
      return SHT21_ERROR_WIRE_ENDTRANSMISSION_UNKNOWN;
  }

  _ReadRawMeasurement(&raw_measurement);

  *rh = _ConvertRelativeHumidity(raw_measurement.data);
  
  return SHT21_OK;
}