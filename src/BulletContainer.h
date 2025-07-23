#ifndef CBULL_CON_H
#define CBULL_CON_H

#include "bullet.h"
#include "TextureLoader.h"

class CBulletContainer
{
    int _count;
    public:
    Bullet* buls;


    CBulletContainer(){ buls = 0; _count=0;}
    int count(){return _count;};
    void add(Bullet& newbulet);
    void removeDead();
    void draw(PicsContainer& pics, int pskx, int psky, int pushx, int pushy, int scrx, int scry, int posx, int posy);
    void destroy();
    ~CBulletContainer(){}


};

 

#endif // CBULL_CON_H

