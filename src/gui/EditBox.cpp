#include "EditBox.h"
#include <cstdio>
#include "Text.h"




void EditBox::init(unsigned int dx, unsigned int dy, const char* dtitl, unsigned char dmaxl)
{
    setpos(dx, dy);
    strcpy(title,dtitl);
    maxlength=dmaxl;
    entered=false;
    canceled=false;
    strcpy(text,"");
    pressedKey = SDL_SCANCODE_RETURN;
}
//------------------------------------
void EditBox::getInput(const char* eventText, unsigned keydown)
{
    if (pressedKey != keydown)
    {
        if ( keydown == SDL_SCANCODE_RETURN)
        {
            entered = true;
        }
        else if ( keydown == SDL_SCANCODE_ESCAPE )
        {
            canceled = true;
        }
        else if (keydown == SDL_SCANCODE_BACKSPACE)
        {
            if (strlen(text))
            {
                text[strlen(text) - 1] = 0;
            }
        }
        else if (strlen(text) < 39 && keydown != 0)
        {
            strcat(text, eventText);
        }
    }

    pressedKey = keydown;

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
    WriteShadedText(getX() + 2, getY() + 1, pics, font, title);

    if ((rand()%100)%5==0)
    {
        char tmp[80];
        sprintf(tmp,"%s_",text);   
        WriteShadedText(getX() + 5, getY() + 16, pics, font,tmp);
    }
    else
    {
        WriteShadedText(getX() + 5, getY() + 16, pics, font,text);
    }

  //coolframe(x,y,ilgis*12,40,remelis,ekranas);

}
//----------------------------------------------
void EditBox::reset()
{
    strcpy(text,"");
    entered=false;
    canceled=false;
    pressedKey = SDL_SCANCODE_RETURN;
}

