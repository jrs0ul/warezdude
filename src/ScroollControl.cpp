#include <cstring>
#include "ScroollControl.h"
#include "WriteText.h"


void ScroollControl::init(unsigned int dx, unsigned int dy,
						  const char *dt, long defstate, long dmaxstate, int dstep){

							  x=dx; y=dy;
							  strcpy(title,dt);
							  state=defstate;
							  maxstate=dmaxstate;
							  step=dstep;
							  selected=false; //dar nieko neisirinkom
							  canceled=false;
							  deactivate();
							  pressedkey=0;

}

void ScroollControl::draw(Picture &rod, Picture &bg, Picture *font, LPD3DXSPRITE &spraitas, LPDIRECT3DDEVICE9 &device){
	WriteText(x,y,spraitas,font,title);
	for (int i=0;i<maxstate/step;i++){
		bg.Blt(spraitas,x+10+i,y+16,2);
	}
	rod.Blt(spraitas,x+10+state/step,y+16+8,1,1.0f,1.0f,1.0f,0.0f,1.0f,1.0f,1.0f,true);

}

void ScroollControl::getInput(unsigned char key){
 if (key!=pressedkey){
  //cia tam kad parinktu kai mygtuka atleidi
  if (pressedkey==13) {
   
    selected=true;      //enter
  }

  if (pressedkey==27) {
   canceled=true;      //esc
  }

  
  
  

	
  pressedkey=key;
 }
  switch(key){
    case VK_LEFT:if (state>step) //up
        state-=step;
      
	   break;

    case VK_RIGHT:if (state<maxstate-step)  //down
        state+=step;
       
	    break;

   // default: pressedkey=0;
   }

}
//----------------------------
void ScroollControl::reset(){
	selected=false;
	canceled=false;
}