#include <cmath>
#include "bullet.h"
#include "Usefull.h"
#include "Dude.h"
#include "Consts.h"


void Bullet::update(const bool** map, int width, int height)
{

    if (!explode)
    {

        if (type != WEAPONTYPE_MINES)
        {
            Vector3D vl = MakeVector(PROJECTILE_BULLET_SPEED, 0, angle);
            float difx = vl.x;
            float dify = vl.y;

            const BulletFrames explosionFrame = (type == WEAPONTYPE_SHRINKER) ? BF_DUKE_SHRINKER_EXPLODE : BF_YELLOW_PEW_EXPLODE;

            if ((y + radius - dify < height * TILE_WIDTH) && (x + radius - dify < width * TILE_WIDTH) && 
                    (x -radius + difx > -16.f) && (y - radius - difx > -16.f))
            {

                if (lifeTime < PROJECTILE_MAX_LIFETIME)
                {

                    int yidx = (int)round((y - (dify * 1.5f)) / TILE_WIDTH);
                    int xidx = (int)round((x + (difx * 1.5f)) / TILE_WIDTH);

                    if ((yidx >= height) || (xidx >= width)) // collide with the edge of the map
                    {
                        frame = explosionFrame;
                        explode = true;
                        return;
                    }

                    if (!map[yidx][xidx])
                    {
                        lifeTime += 2;

                        x += difx;
                        y -= dify;
                    }
                    else  //  bump into obstacle
                    {
                        frame = explosionFrame;
                        explode = true;

                    }
                }
                else  //  end of lifetime
                {
                    frame = explosionFrame;
                    explode = true;
                }
            }
            else
            {

                frame = explosionFrame;
                explode = true;
            }
        }  //  not mines
    }
    else
    {
        explodetim++;

        if (explodetim >= PROJECTILE_EXPLOSION_DURATION)
        {
            explodetim = 0;
            lifeTime = 0;
            exists = false;
            explode = false;
            frame = BF_YELLOW_PEW;
        }
    }
}

bool Bullet::onHit(DArray<Dude>& dudes)
{
    int dmg = PROJECTILE_BULLET_DAMAGE;

    if (type == WEAPONTYPE_MINES)
    {
        dmg = PROJECTILE_MINE_DAMAGE;
    }

    bool hit = false;
    int tmpID = 0;


    for (unsigned i = 0; i < dudes.count(); i++)
    {
        if (CirclesColide(dudes[i].x, dudes[i].y, 8, x, y, 8))
        {

            tmpID = dudes[i].id;

            if ((parentID != tmpID) && //bullet should not hit the shooter
                (!dudes[i].shot) &&
                (!dudes[i].spawn))
            {

                if (type == WEAPONTYPE_SHRINKER)
                {
                    if (!dudes[i].shrinked)
                    {
                        dudes[i].shrinked = true;
                        hit = true;
                    }
                }
                else
                {
                    if (!hit)
                    {
                        hit = true;
                    }

                    dudes[i].hit = true;
                    dudes[i].damage(dmg);
                    dudes[i].lastDamagedBy = parentID;
                }
            }
        }
    }

    return hit;

}

