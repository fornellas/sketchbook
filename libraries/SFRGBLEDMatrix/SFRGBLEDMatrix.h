#ifndef SFRGBLEDMatrix_h
#define SFRGBLEDMatrix_h

#include <Arduino.h>

//
// Display properties
//

/*

SQUARE: Max 4 displays
            +--> 0x0
           /
           +---+---+ --> 15x0
           | 3 | 2 |<=\\
           +---+---+  ||
 Wires ==> | 0 | 1 |==//
           +---+---+ --> 15x15

SIDE: Max 8 displays

            +--> 0x0
           /
           +---+---+---+---+ --> 31x0
 Wires ==> | 0 | 1 | 2 | 3 |
           +---+---+---+---+ --> 31x7

*/

#define SQUARE true
#define SIDE false
#define DISP_LEN byte(8)

//
// Colors
//

// | 0000 RRRR | GGGG BBBB |
// |   byte0   |   byte1   |

typedef uint16_t Color;

#define BITS_PER_COLOR 4
#define MAX_C 15
#define MID_C 7

#define RGB(r,g,b) (((r)<<(BITS_PER_COLOR*2))|((g)<<BITS_PER_COLOR)|(b))

#define GET_RED(c) ((c&(0x0F<<(BITS_PER_COLOR*2)))>>(BITS_PER_COLOR*2))
#define GET_GREEN(c) ((c&(0x0F<<BITS_PER_COLOR))>>BITS_PER_COLOR)
#define GET_BLUE(c) (c&0x0F)

#define	BLACK		RGB(0,	  0,    0    )
#define WHITE		RGB(MAX_C,MAX_C,MAX_C)

#define RED		RGB(MAX_C,0,    0    )
#define RED_MAGENTA	RGB(MAX_C,0,    MID_C) // PINK
#define RED_YELLOW	RGB(MAX_C,MID_C,0    ) // ORANGE

#define GREEN		RGB(0,    MAX_C,0    )
#define GREEN_CYAN	RGB(0,    MAX_C,MID_C)
#define GREEN_YELLOW	RGB(MID_C,MAX_C,0    )

#define BLUE		RGB(0,    0,    MAX_C)
#define BLUE_CYAN	RGB(0,    MID_C,MAX_C)
#define BLUE_MAGENTA	RGB(MID_C,0,    MAX_C) // WINE?

#define CYAN		RGB(0,    MAX_C,MAX_C)
#define MAGENTA		RGB(MAX_C,0,    MAX_C)
#define YELLOW		RGB(MAX_C,MAX_C,0    )

//
// Class
//

class SFRGBLEDMatrix {
  private:
    byte pinSS;
    void sendChar(byte cData);
    byte dispCountSqrt;
    byte *frameBuff;
    uint16_t buffSize;
    void paintColor(uint8_t x, uint8_t y, uint16_t colorOffset, uint8_t value);
  public:
    // Useful variables
    bool square;
    byte dispCount;
    byte width;
    byte height;
    uint16_t pixels;
    // Constructor / destructor
    SFRGBLEDMatrix(
      bool square,
      byte dispCount, // must be equals 4 if square equals true
      byte pinSS); // CS pin
    ~SFRGBLEDMatrix();
    // Must be called before show() if SPI configuration was changed after SFRGBLEDMatrix()
    void setupSPI();
    // Must be called before show() if SPI / SS pins configuration were changed after SFRGBLEDMatrix()
    void setupPINs(); 
    // Send buffer to the screens
    void show(); 
    // 4 pixels height font
    void printChar4p(char c, Color color, int x, int y);
    void printString4p(char *s, Color color, int x, int y);
    // paint single pixel
    void paintPixel(Color color, int x, int y);
    Color getPixel(int x, int y);
    // fill screen with one color
    void fill(Color color);
    // same as fill(BLACK), only faster
    void clear();
};

#endif
