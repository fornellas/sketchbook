/*

Facilities
- LED Matrix
- Buttons
- RTC
- LCD
- Humidity
- Temperature outdoor
- Temperature indoor
- Pressure
- Light

Modes
- Clock
- Fire
- Lamp
- Plasma
- Equalizer
  - Line In
  - Mic
- Rainbow / Colorspace show

Other
- Data logging / graphing

Bugs

- Read buttons inside interrupt to avoid loosing events
- fill(BLACK) em Mode::Mode as vezes deixa alguns pontos coloridos na tela

TODO
- Resolver problema de imagem corrompida no Plasma (parece ser falta de memoria)
- Unificar spectrum em classe
- Dew point
- GOL
- 1D celular automata
- Matrix mode
- Demo mode.
- Video streamming via HTTP.
- display XPM images
- Plasma
  - Save pixmap state
  - Change pixmap overtime: distribute fillPlasma() load over main loop, than fade old pixmap to new pimap.
- Mode change animation
  - Text scroll?
  - Icon?
- Clock
  - Binary
  - Tokyo flash style
    - 2 variants
  - Consolidate DX modes in one method
- Fire
  - tunnable palette
- Temperatue
  - Current
  - Indoor / outdoor / barometric
  - Max / min
  - Plot
- HW
  - reset Matrices with Arduino.
- Weather icons
  - clear
  - cloudy
  - rain
  - day / night
    - sky color
    - sun / moon
- Mode change animation
- Anti-aliased scroll
- RTC lib
  - NTP
  - Detect unconfigured time
  - Detect disconected RTC

*/
