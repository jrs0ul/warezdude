#ifndef CBULL_CON_H
#define CBULL_CON_H


class Bullet;
class PicsContainer;

class CBulletContainer
{
    int _count;
    public:
    Bullet* buls;


    CBulletContainer(){ buls = 0; _count=0;}
    int count(){return _count;};
    void add(Bullet& newbulet);
    void removeDead();
    void draw(PicsContainer& pics, float posx, float posy);
    void destroy();
    ~CBulletContainer(){}


};

 

#endif // CBULL_CON_H

