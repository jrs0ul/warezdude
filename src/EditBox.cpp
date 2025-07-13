#include <cstdio>
#include "WriteText.h"
#include "EditBox.h"




void EditBox::init(unsigned int dx, unsigned int dy, const char* dtitl, unsigned char dmaxl){
 x=dx;
 y=dy;
 strcpy(title,dtitl);
 maxlength=dmaxl;
 entered=false;
 canceled=false;
 strcpy(text,"");
 pressedKey=0;
}
//------------------------------------
void EditBox::getInput(unsigned char key, unsigned char Gkey){
 if (pressedKey!=key){
  if (Gkey==13) entered=true;
  if (Gkey==27) canceled=true;
  int ilgis=(int)strlen(text);
  pressedKey=key;
  if ((key>31)&&(key<127)){
   if (ilgis<maxlength){
    text[ilgis]=key;
    text[ilgis+1]='\0'; 
   }
  }
   else

	if (key==8){
		if (ilgis>0){
      text[ilgis-1]='\0';
	 }
    
    else if (key!=13) pressedKey=0;
	}
  
 }
}
//------------------------------------
void EditBox::draw(LPDIRECT3DDEVICE9& device,LPD3DXSPRITE& ekranas, Picture* font,Picture* remelis){
 
 int ilgis;
 if (strlen(title)>maxlength)
  ilgis=(int)strlen(title);
 else
  ilgis=maxlength;

  //DrawBlock(device,ekranas,x,y,ilgis*12,40,0,0,125);
  WriteText(x+2,y+1,ekranas,font,title);

  if ((rand()%100)%5==0){
   char tmp[80];
   sprintf(tmp,"%s_",text);	  
   WriteText(x+5,y+16,ekranas,font,tmp);
  }
  else
   WriteText(x+5,y+16,ekranas,font,text);
  
  //coolframe(x,y,ilgis*12,40,remelis,ekranas);
  
 
}
//----------------------------------------------
void EditBox::reset(){
 strcpy(text,"");
 entered=false;
 canceled=false;
 pressedKey=0;
}

