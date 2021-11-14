#ifndef I2C_CLEARBUS_h
#define I2C_CLEARBUS_h

// Taken from http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html

enum I2CClearBusError {
	I2C_CLEARBUS_OK,
	I2C_CLEARBUS_ERROR_SCL_HELD_LOW,
	I2C_CLEARBUS_ERROR_SDA_HELD_LOW_BY_SLAVE_CLOCK_STRETCH_FOR_MORE_THAN_2SEC,
	I2C_CLEARBUS_ERROR_SDA_HELD_LOW_AFTER_20_CLOCKS,
};

enum I2CClearBusError I2CClearBus(int sda, int scl);

#endif