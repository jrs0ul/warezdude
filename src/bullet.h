#ifndef BULLET_H
#define BULLET_H


class Bullet
{
public:
    float x;
    float y;
    float angle;
    float radius;
    int   tim;
    int   explodetim;
    int   parentID;
    int   frame;
    bool  explode;
    bool  exists;
    bool  isMine;



    Bullet()
    : x(0.f)
    , y(0.f)
    , angle(0.f)
    , radius(4.f)
    , tim(0)
    , explodetim(0)
    , parentID(0)
    , frame(0)
    , explode(false)
    , exists(true)
    , isMine(false)
    {
    }
    void ai(bool** map, int width, int height);
};





#endif //BULLET_H
