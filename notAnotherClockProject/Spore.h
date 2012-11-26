#ifndef Spore_h
#define Spore_h

#include "Mode.h"
#include <SFRGBLEDMatrix.h>

class Spore: 
public Mode {
  private:
    class Cell {
      private:
        int x, y;
      public:
        Cell(int xin, int yin);
        void run();
        void move(int dx, int dy);
    };
    static void setpix(int x, int y, Color c);
    static Color getpix(int x, int y);
    int numcells;
    Cell **cells;
  public:
    Spore();
    inline void loop();
    ~Spore();
};

#endif
