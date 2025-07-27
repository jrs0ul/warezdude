/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2010
 -------------------------------------------
 SDL window
 mod. 2010.09.27
 */

#include "SDLVideo.h"
#include <cstdio>

bool SDLVideo::InitWindow(const char * title, const char * iconPath,
                          bool isWindowed, bool resizable){


    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
        return false;

    int rSize, gSize, bSize, aSize;
    rSize = gSize = bSize = aSize = 8;
    int buffsize = ((rSize + gSize + bSize + aSize + 7) & 0xf8);

    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 0 );
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, rSize );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, gSize );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, bSize );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, aSize );
    SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, buffsize );

    _flags = SDL_WINDOW_OPENGL;

    if (!isWindowed){
        _flags = _flags |SDL_WINDOW_FULLSCREEN;
    }

    icon = SDL_LoadBMP(iconPath);
   

    mainWindow = SDL_CreateWindow(title, 
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         _width,
                         _height,
                         _flags );
    if( mainWindow == 0 )
    {
        puts("can't set video mode");
        return false;
    }

    if (icon)
    {
        //SDL_SetColorKey(icon, SDL_SRCCOLORKEY,
        //                SDL_MapRGB(icon->format, 255, 255, 255));
        SDL_SetWindowIcon(mainWindow, icon);
    }

    SDL_GL_CreateContext(mainWindow);

    printf("Context created\n");

    return true;


}
//-------------------------------------------
void SDLVideo::setMetrics(unsigned w, unsigned h){
    _width = w;
    _height = h;
}

//--------------------------------------------
void SDLVideo::Quit(){

    SDL_Quit();
}

void SDLVideo::swap()
{
    SDL_GL_SwapWindow(mainWindow);
}
