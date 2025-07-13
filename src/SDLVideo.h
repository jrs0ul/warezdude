/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2010
 -------------------------------------------
 SDL window
 mod. 2010.09.27
 */

#ifndef _SDL_VIDEO_WRAP_H
#define _SDL_VIDEO_WRAP_H

#ifdef _WIN32
    #ifdef _MSC_VER
        #include <SDL.h>
    #else
        #include <SDL2/SDL.h>
    #endif
#else
  #include <SDL2/SDL.h>
#endif

class SDLVideo{

    int _bpp;
    int _flags;

    unsigned _width;
    unsigned _height;

    SDL_Surface * icon;
    SDL_Window* mainWindow;

public:

    SDLVideo(){_width = 640; _height = 480;}

    unsigned width(){return _width;}
    unsigned height(){return _height;}
    int      colorBits(){return _bpp;}
    int      flags(){return _flags;}
    void     setMetrics(unsigned w, unsigned h);

    bool     InitWindow(const char * title, const char * iconPath,
                    bool isWindowed = true, bool resizable = false);
    void     Quit();
    void     swap();

};


#endif //_SDL_VIDEO_WRAP_H

