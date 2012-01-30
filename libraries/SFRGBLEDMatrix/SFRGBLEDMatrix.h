#ifndef SFRGBLEDMatrix_h
#define SFRGBLEDMatrix_h

#include <Arduino.h>

#define SQUARE true
#define SIDE false

#define DISP_LEN byte(8)

// 7 >= r >= 0
// 7 >= g >= 0
// 2 >= r >= 0
#define RGB(r,g,b) ((r<<5)|(g<<2)|b)

#define MAX_R	7
#define MID_R	4
#define MAX_G	7
#define MID_G	4
#define MAX_B	3
#define MID_B	2

#define	BLACK		RGB(0,	  0,    0    )
#define WHITE		RGB(MAX_R,MAX_G,MAX_B)

#define RED		RGB(MAX_R,0,    0    )
#define RED_MAGENTA	RGB(MAX_R,0,    MID_B) // PINK
#define RED_YELLOW	RGB(MAX_R,MID_G,0    ) // ORANGE

#define GREEN		RGB(0,    MAX_G,0    )
#define GREEN_CYAN	RGB(0,    MAX_G,MID_B)
#define GREEN_YELLOW	RGB(MID_R,MAX_G,0    )

#define BLUE		RGB(0,    0,    MAX_B)
#define BLUE_CYAN	RGB(0,    MID_G,MAX_B)
#define BLUE_MAGENTA	RGB(MID_R,0,    MAX_B) // WINE?

#define CYAN		RGB(0,    MAX_G,MAX_B)
#define MAGENTA		RGB(MAX_R,0,    MAX_B)
#define YELLOW		RGB(MAX_R,MAX_G,0    )

class SFRGBLEDMatrix {
  private:
    byte pinSS;
    void sendChar(byte cData);
    byte dispCountSqrt;
    void setupSPI();
  public:
    bool square;
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
    byte dispCount;
    byte width;
    byte height;
    word pixels;
    byte *frameBuff;
    SFRGBLEDMatrix(
      bool square,
      byte dispCount, // must be equals 4 if square equals true
      byte pinSS); // CS
    ~SFRGBLEDMatrix();
    void show(); // send frameBuff to the screens
    void config(); // configure daisy chain
    void printChar4p(char c, byte color, int x_offset, int y_offset); // print 4x4 char
    void paintPixel(byte color, int x_offset, int y_offset); // paint single pixel
    void fill(byte color); // fill screen with one color
};

#endif
