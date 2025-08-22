#ifndef BULLET_H
#define BULLET_H


#include "DArray.h"
#include "WeaponTypes.h"

class Dude;


enum BulletFrames
{
    BF_YELLOW_PEW,
    BF_MINE,
    BF_YELLOW_PEW_EXPLODE,
    BF_DUKE_SHRINKER,
    BF_DUKE_SHRINKER_EXPLODE
};

class Bullet
{
public:
    float        x;
    float        y;
    float        angle;
    float        radius;
    int          lifeTime;
    int          explodetim;
    int          parentID;
    BulletFrames frame;
    WeaponTypes  type;
    bool         explode;
    bool         exists;

    Bullet()
    : x(0.f)
    , y(0.f)
    , angle(0.f)
    , radius(4.f)
    , lifeTime(0)
    , explodetim(0)
    , parentID(0)
    , frame(BF_YELLOW_PEW)
    , type(WEAPONTYPE_REGULAR)
    , explode(false)
    , exists(true)
    {
    }

    void update(const bool** map, int width, int height);
    bool onHit(DArray<Dude>& dudes);
};





#endif //BULLET_H
