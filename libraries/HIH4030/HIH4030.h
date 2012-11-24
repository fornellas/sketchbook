#ifndef HIH4030_h
#define HIH4030_h

#include <Humidity.h>
#include <Temperature.h>

class HIH4030:
public Humidity {
private:
  uint8_t pin;
  Temperature *temperature;
public:
  HIH4030(uint8_t sensorPin, Temperature *temperature); // 5V Arduinos only!
  // Sensor
  void loadFromSensor();
};

#endif
