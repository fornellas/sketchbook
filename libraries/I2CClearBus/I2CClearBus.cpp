#include <I2CClearBus.h>
#include <Arduino.h>


// This routine turns off the I2C bus and clears it
// on return SCA and SCL pins are tri-state inputs.
// You need to call Wire.begin() after this to re-enable I2C
// This routine does NOT use the Wire library at all.
enum I2CClearBusError I2CClearBus(int sda, int scl) {
#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

  pinMode(sda, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(scl, INPUT_PULLUP);

  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
  // up of the DS3231 module to allow it to initialize properly,
  // but is also assists in reliable programming of FioV3 boards as it gives the
  // IDE a chance to start uploaded the program
  // before existing sketch confuses the IDE by sending Serial data.

  boolean SCL_LOW = (digitalRead(scl) == LOW); // Check is SCL is Low.
  if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master. 
    return I2C_CLEARBUS_ERROR_SCL_HELD_LOW; //I2C bus error. Could not clear SCL clock line held low
  }

  boolean SDA_LOW = (digitalRead(sda) == LOW);  // vi. Check SDA input.
  int clockCount = 20; // > 2x9 clock

  while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
    clockCount--;
  // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(scl, INPUT); // release SCL pullup so that when made output it will be LOW
    pinMode(scl, OUTPUT); // then clock SCL Low
    delayMicroseconds(10); //  for >5uS
    pinMode(scl, INPUT); // release SCL LOW
    pinMode(scl, INPUT_PULLUP); // turn on pullup resistors again
    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS
    // The >5uS is so that even the slowest I2C devices are handled.
    SCL_LOW = (digitalRead(scl) == LOW); // Check if SCL is Low.
    int counter = 20;
    while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(scl) == LOW);
    }
    if (SCL_LOW) { // still low after 2 sec error
      return I2C_CLEARBUS_ERROR_SDA_HELD_LOW_BY_SLAVE_CLOCK_STRETCH_FOR_MORE_THAN_2SEC; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    }
    SDA_LOW = (digitalRead(sda) == LOW); //   and check SDA input again and loop
  }
  if (SDA_LOW) { // still low
    return I2C_CLEARBUS_ERROR_SDA_HELD_LOW_AFTER_20_CLOCKS; // I2C bus error. Could not clear. SDA data line held low
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(sda, INPUT); // remove pullup.
  pinMode(sda, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(sda, INPUT); // remove output low
  pinMode(sda, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10); // x. wait >5uS
  pinMode(sda, INPUT); // and reset pins as tri-state inputs which is the default state on reset
  pinMode(scl, INPUT);
  return I2C_CLEARBUS_OK; // all ok
}