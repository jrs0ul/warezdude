#ifndef _EDITBOX_H_
#define _EDITBOX_H_

#include "Picture3D.h"
#include "ActiveControl.h"



class EditBox:public ActiveControl{
 public:
  unsigned char maxlength;
  char text[40];
  bool entered;
  bool canceled;
  char title[20];
  unsigned char pressedKey;
  void init(unsigned int dx, unsigned int dy, const char* dtitl, unsigned char dmaxl);
  void getInput(unsigned char key, unsigned char Gkey);
  void draw(LPDIRECT3DDEVICE9& device,LPD3DXSPRITE& ekranas, Picture* font,Picture* remelis=0);
  void reset();
 };

#endif //_EDITBOX_H_