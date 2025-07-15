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
                int pskx, int scrx, int psky, int scry,
                int pushx, int posx, int pushy, int posy);
};


#endif //DECAL_H
