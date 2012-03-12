#ifndef Mode_h
#define Mode_h

class Mode {
public:
  Mode();
  ~Mode();
  // Name
  PGM_P *getName();
  // Thil will be called in a loop
  virtual void loop() = 0;
};

#endif

