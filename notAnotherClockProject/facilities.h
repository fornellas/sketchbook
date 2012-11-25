#ifndef facilities_h
#define facilities_h

#include <SFRGBLEDMatrix.h>
#include <U8glib.h>
#include "Light.h"
#include <Time.h>
#include "tz.h"
#include <OneWire.h>
#include <DS18S20.h>
#include <Temperature.h>
#include <Pressure.h>
#include <BMP085.h>
#include <Humidity.h>
#include <HIH4030.h>
#include <DHT22.h>
#include "Net.h"
#include "Button.h"
#include "EEPROM_addr.h"
#include "pins.h"

// Sparkfun RGB LED Matrix
extern SFRGBLEDMatrix *ledMatrix;

// LCD12864
extern U8GLIB_ST7920_128X64 *lcd;

// Light
extern Light *light;

// Time
extern time_t bootTime;

// OneWire
extern OneWire *ow;

// DS18S20
extern DS18S20 *ds18s20;
extern Temperature& temperatureOutside2;

// BMP085
extern BMP085 *bmp085;
extern Pressure& pressure;
extern Temperature& temperatureInside;

// HIH4030
extern HIH4030 *hih4030;
extern Humidity& humidityInside;

// DHT22
extern DHT22 *dht22;
extern Temperature& temperatureOutside;
extern Humidity& humidityOutside;

// Net
extern Net *net;

#endif
