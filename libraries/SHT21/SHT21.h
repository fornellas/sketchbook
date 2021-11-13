#ifndef SHT21_h
#define SHT21_h

#include "Arduino.h"
#include "Wire.h"
#include <stdint.h>

enum SHT21Error {
  SHT21_OK,
  SHT21_ERROR_WIRE_WRITE,
  SHT21_ERROR_WIRE_ENDTRANSMISSION_LENGTH_TO_LONG_FOR_BUFFER,
  SHT21_ERROR_WIRE_ENDTRANSMISSION_ADDRESS_SEND_NACK_RECEIVED,
  SHT21_ERROR_WIRE_ENDTRANSMISSION_DATA_SEND_NACK_RECEIVED,
  SHT21_ERROR_WIRE_ENDTRANSMISSION_OTHER_TWI_ERROR,
  SHT21_ERROR_WIRE_ENDTRANSMISSION_TIMEOUT,
  SHT21_ERROR_WIRE_ENDTRANSMISSION_UNKNOWN,
  SHT21_ERROR_WIRE_REQUESTFROM,
  SHT21_ERROR_CHECKSUM,
};

class SHT21 {
  public:
  
  SHT21Error begin(TwoWire *wire = &Wire);
  SHT21Error GetTemperature(double *t);
  SHT21Error GetRelativeHumidity(double *temperature);
  SHT21Error SoftReset();
  
  private:

  SHT21Error _CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum);
  SHT21Error _ReadRawMeasurement(struct RawMeasurement *raw_measurement);
  double _ConvertTemperature(uint16_t data);
  double _ConvertRelativeHumidity(uint16_t data);

  TwoWire* _wire;
};

#endif