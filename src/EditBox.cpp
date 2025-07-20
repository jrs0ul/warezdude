#include <cstdio>
#include "gui/Text.h"
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
void EditBox::getInput(const char* eventText, unsigned key)
{
    if (pressedKey!=key){
        if ( pressedKey == SDL_SCANCODE_RETURN )
            entered = true;
        if ( pressedKey == 27 )
            canceled = true;
    }

    strcpy(text, eventText);

    //unsigned ilgis = (unsigned)strlen(text);
    pressedKey = key;
    /*if ((key > 31) && (key < 127)){
        if (ilgis < maxlength){
            text[ilgis] = (char)key;
            text[ilgis + 1] = '\0';
        }
    }
    else
        if (key==8){ //backspace
            if (ilgis>0){
                text[ilgis-1]='\0';
            }
            else 
                if (key!=13) 
                    pressedKey=0;
        }
    }*/

}
//------------------------------------
void EditBox::draw(PicsContainer& pics, unsigned font, unsigned remelis)
{

    int ilgis;
    if (strlen(title)>maxlength)
    {
        ilgis=(int)strlen(title);
    }
    else
    {
        ilgis=maxlength;
    }

    //DrawBlock(device,ekranas,x,y,ilgis*12,40,0,0,125);
    WriteText(x+2,y+1, pics, font, title);

    if ((rand()%100)%5==0)
    {
        char tmp[80];
        sprintf(tmp,"%s_",text);   
        WriteText(x+5,y+16, pics, font,tmp);
    }
    else
    {
        WriteText(x+5,y+16, pics, font,text);
    }

  //coolframe(x,y,ilgis*12,40,remelis,ekranas);

}
//----------------------------------------------
void EditBox::reset(){
 strcpy(text,"");
 entered=false;
 canceled=false;
 pressedKey=0;
}

