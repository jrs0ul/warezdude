#ifndef BULLET_H
#define BULLET_H


#include "DArray.h"
#include "WeaponTypes.h"

class Dude;

class Bullet
{
public:
    float       x;
    float       y;
    float       angle;
    float       radius;
    int         tim;
    int         explodetim;
    int         parentID;
    int         frame;
    WeaponTypes type;
    bool        explode;
    bool        exists;

    Bullet()
    : x(0.f)
    , y(0.f)
    , angle(0.f)
    , radius(4.f)
    , tim(0)
    , explodetim(0)
    , parentID(0)
    , frame(0)
    , type(WEAPONTYPE_REGULAR)
    , explode(false)
    , exists(true)
    {
    }

    void update(const bool** map, int width, int height);
    bool onHit(DArray<Dude>& dudes);
};





#endif //BULLET_H
