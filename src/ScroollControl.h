#ifndef SCROLL_CONTROL_H
#define SCROLL_CONTROL_H
#include "ActiveControl.h"
#include "TextureLoader.h"

class ScroollControl:public ActiveControl{
public:
  char title[30]; //antraste
  bool selected;  //ar pasirinkta
  bool canceled;  //ar atshauktas
  unsigned char pressedkey;
  long state;
  long maxstate;
  int step;
   
  //priskiriam data 
  void init(unsigned int dx, unsigned int dy, const char* dt, long defstate, long dmaxstate, int dstep);
  //numusa userio atlikta pasirinkima
  void reset();
  //jei keyus i virsu apcia tai keiciasi ir state
  void getInput(const unsigned char* keys, const unsigned char* oldKeys);
  //nupaisom
  void draw( PicsContainer& pics, unsigned rod, unsigned bg,  unsigned font);
};

#endif //SCROLL_CONTROL_H
