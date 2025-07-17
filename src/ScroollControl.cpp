#include <cstring>
#include "ScroollControl.h"
#include "gui/Text.h"


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

void ScroollControl::draw(PicsContainer& pics, unsigned rod, unsigned bg, unsigned font)
{
    WriteText(x, y, pics, font, title);

    for (int i=0;i<maxstate/step;i++)
    {
        pics.draw(bg, x+10+i,y+16,2);
    }

    pics.draw(rod, x+10+state/step, y+16+8, 1, true, 1.0f,1.0f, 0.0f, COLOR(1.0f,1.0f,1.0f, 1.f), COLOR(1.f,1.f,1.f,1.f));

}

void ScroollControl::getInput(unsigned key){
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
    case SDLK_LEFT:if (state>step) //up
        state-=step;
      
       break;

    case SDLK_RIGHT:
    {
        if (state<maxstate-step)  //down
            state+=step;
    }
       
        break;

   // default: pressedkey=0;
   }

}
//----------------------------
void ScroollControl::reset(){
    selected=false;
    canceled=false;
}
