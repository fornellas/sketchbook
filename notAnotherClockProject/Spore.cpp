#include "Spore.h"
#include "facilities.h"

#define MAXCELLS 12
#define SPORE_COLOR RGB(10, 15, 8)
// set lower for smoother animation, higher for faster simulation
#define RUNS_PER_LOOP 2

Spore::Cell::Cell(int xin, int yin){
  x = xin;
  y = yin;
}

void Spore::Cell::run(){
  // Fix cell coordinates
  while (x < 0) {
    x+=ledMatrix->width;
  }
  while (x > ledMatrix->width - 1) {
    x-=ledMatrix->width;
  }
  while (y < 0) {
    y+=ledMatrix->height;
  }
  while (y > ledMatrix->height - 1) {
    y-=ledMatrix->height;
  }

  // Cell instructions
  if (getpix(x + 1, y) == BLACK) {
    move(0, 1);
  } 
  else if (getpix(x, y - 1) != BLACK && getpix(x, y + 1) != BLACK) {
    move((int)random(9) - 4, (int)random(9) - 4);
  }
}

void Spore::Cell::move(int dx, int dy){
  if (getpix(x + dx, y + dy) == BLACK) {
    setpix(x + dx, y + dy, getpix(x, y));
    setpix(x, y, BLACK);
    x += dx;
    y += dy;
  }
}

void Spore::setpix(int x, int y, Color c){
  while (x < 0) x+=ledMatrix->width;
  while (x > ledMatrix->width - 1) x-=ledMatrix->width;
  while (y < 0) y+=ledMatrix->height;
  while (y > ledMatrix->height - 1) y-=ledMatrix->height;
  ledMatrix->paintPixel(c, x, y);
}

Color Spore::getpix(int x, int y){
  while (x < 0) x+=ledMatrix->width;
  while (x > ledMatrix->width - 1) x-=ledMatrix->width;
  while (y < 0) y+=ledMatrix->height;
  while (y > ledMatrix->height - 1) y-=ledMatrix->height;
  return ledMatrix->getPixel(x, y);
}

Spore::Spore():
Mode(PSTR("Spore")){
  numcells=0;
  cells = (Cell **)malloc(MAXCELLS*sizeof(Cell *));
  // Add cells at random places
  for (int i = 0; i < MAXCELLS; i++) {
    int cX = (int)random(ledMatrix->width);
    int cY = (int)random(ledMatrix->height);
    if (getpix(cX, cY) == BLACK) {
      setpix(cX, cY, SPORE_COLOR);
      cells[numcells] = new Cell(cX, cY);
      numcells++;
    }
  }
}

void Spore::loop(){
  // Run cells in random order
  for (int i = 0; i < RUNS_PER_LOOP; i++) {
    int selected = min((int)random(numcells), numcells - 1);
    cells[selected]->run();
  }
  ledMatrix->show();
}

Spore::~Spore(){
  for (int i = 0; i < MAXCELLS; i++)
    delete cells[i];
  free(cells);
}
