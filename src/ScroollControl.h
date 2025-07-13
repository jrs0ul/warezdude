#ifndef SCROLL_CONTROL_H
#define SCROLL_CONTROL_H
#include "ActiveControl.h"

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
  void getInput(unsigned char key);
  //nupaisom
  void draw( Picture& rod, Picture& bg,  Picture* font,  LPD3DXSPRITE& spraitas,LPDIRECT3DDEVICE9& device);
};

#endif //SCROLL_CONTROL_H
