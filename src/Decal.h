#ifndef DECAL_H
#define DECAL_H


#include "TextureLoader.h"

class Decal{
public:
    int x,y;
    float alpha;
    float r,g,b;
    int frame;

    Decal(){
        x=y=0;
        alpha=1.0f;
        r=g=b=1.0f;
        frame=0;
    }
    void draw(PicsContainer& pics, unsigned index,
                float posx, float posy,
                int ScreenWidth, int ScreenHeight);
};


#endif //DECAL_H
