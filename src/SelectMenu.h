#ifndef	_SELECTMENIU_H_
#define _SELECTMENIU_H_

#include <cstring>
#include "ActiveControl.h"
#include "TextureLoader.h"

struct Smenu{
  char opt[20][30];
  unsigned char pics[20];
  unsigned char ids[20];
  unsigned char count;
  Smenu(){memset(pics,0,20); memset(ids,0,20);}
 };

//--------------------------------------





 class SelectMeniu:public ActiveControl{ //selecto objectas
 public:
  char title[30];  //antraste
  Smenu selection;   //meniu
  unsigned int width;        //plotis
  unsigned int height;       //aukstis
  unsigned char defstate;     //defaultine opcija
  unsigned char state;        //parinkta tuo metu opcija
  
  bool selected;  //jau viskas pasirinkta, enter paspaustas
  bool canceled;  //ar menu atshauktas
  unsigned pressedkey;
   

  //priskiriam data dx,dy:lango metrikos dst:defstate
  void init(unsigned int dx, unsigned int dy, const char* dt, Smenu& dsel, unsigned char dst,unsigned int dheight=0);
  //numusa userio atlikta pasirinkima
  void reset();
  //jei keyus i virsu apcia tai keiciasi ir state
  void getInput(const unsigned char* keys, const unsigned char* oldKeys);
  //nupaisom
  void draw( PicsContainer& pics, unsigned rod,  unsigned font, unsigned icons=0);
 
 };

#endif
