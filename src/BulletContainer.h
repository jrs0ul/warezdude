#pragma once

#include <vector>
#include "bullet.h"

class SpriteBatcher;
class Dude;

class CBulletContainer
{
public:
    std::vector<Bullet> buls;


    CBulletContainer(){}
    int count(){return buls.size();};
    void add(Bullet& newbulet);
    void draw(SpriteBatcher& pics, float posx, float posy, int ScreenWidth, int ScreenHeight);
    void update(const bool** colisionGrid, std::vector<Dude>& dudes, int mapWidth, int mapHeight);
    void destroy();
    ~CBulletContainer(){}
private:
    void removeDead();


};

