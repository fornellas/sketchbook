#ifndef EthernetInterrupt_h
#define EthernetInterrupt_h

#include <stdint.h>
#include <Arduino.h>

// some parts imported from libraries/Ethernet/utility/w5100.h

#define __GP_REGISTER8(name, address)             \
  static inline void write##name(uint8_t _data) { \
    write(address, _data);                        \
  }                                               \
  static inline uint8_t read##name() {            \
    return read(address);                         \
  }

namespace EthernetInterrupt {
  static uint8_t w5100int;
  static uint8_t interrupt;
  void w5100IntProc();
  uint8_t read(uint16_t addr);
  uint8_t write(uint16_t _addr, uint8_t _data);
  __GP_REGISTER8 (IMR,    0x0016);    // Interrupt Mask
  __GP_REGISTER8(S0_IR,  0x0402);    // Socket 0 Interrupt
  __GP_REGISTER8(S1_IR,  0x0502);    // Socket 1 Interrupt
  __GP_REGISTER8(S2_IR,  0x0602);    // Socket 2 Interrupt
  __GP_REGISTER8(S3_IR,  0x0702);    // Socket 3 Interrupt
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  inline static void initSS()    { DDRB  |=  _BV(4); };
  inline static void setSS()     { PORTB &= ~_BV(4); };
  inline static void resetSS()   { PORTB |=  _BV(4); };
#elif defined(__AVR_ATmega32U4__)
  inline static void initSS()    { DDRB  |=  _BV(6); };
  inline static void setSS()     { PORTB &= ~_BV(6); };
  inline static void resetSS()   { PORTB |=  _BV(6); };
#elif defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB162__)
  inline static void initSS()    { DDRB  |=  _BV(0); };
  inline static void setSS()     { PORTB &= ~_BV(0); };
  inline static void resetSS()   { PORTB |=  _BV(0); };
#else
  inline static void initSS()    { DDRB  |=  _BV(2); };
  inline static void setSS()     { PORTB &= ~_BV(2); };
  inline static void resetSS()   { PORTB |=  _BV(2); };
#endif
  // Interrupt number is different for each board:
  // Duemilanove, Uno, Ethernet
  //   interrupt=0
  // Leonardo
  //   interrupt=1
  // Due
  //   interrupt=2
  // Do not forget to solder the jumper INT on the Ethernet shield!
  // NOTE: on R3 of Ethernet shield, the jumper is unlabeled, and is located
  // below the board, at the left of the word "SHIELD".
  void begin(uint8_t interrupt);
  uint8_t available();
  void next();
}

#endif
