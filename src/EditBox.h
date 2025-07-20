#ifndef _EDITBOX_H_
#define _EDITBOX_H_

#include "ActiveControl.h"
#include "TextureLoader.h"



class EditBox:public ActiveControl{
 public:
  unsigned char maxlength;
  char text[40];
  bool entered;
  bool canceled;
  char title[20];
  unsigned char pressedKey;
  void init(unsigned int dx, unsigned int dy, const char* dtitl, unsigned char dmaxl);
  void getInput(const char* eventText, unsigned key);
  void draw(PicsContainer& pics, unsigned font, unsigned remelis=0);
  void reset();
 };

#endif //_EDITBOX_H_
