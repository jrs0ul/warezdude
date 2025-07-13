#include "WriteText.h"

void WriteText(unsigned int x,unsigned int y, 
			   LPD3DXSPRITE& spraitas, Picture* font,
			   const char* s,
			   float alpha,
			   float scalex, float scaley,
			   float r, float g, float b){

 for (unsigned int i=0;i<strlen(s);i++){
  font->Blt(spraitas,x+(11*scaley)*i,y,s[i]-32,alpha,scalex,scaley,0,r,g,b);
 }
 
}