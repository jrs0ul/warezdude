#include <cmath>
#include "bullet.h"
#include "Usefull.h"
#include "Dude.h"
#include "Consts.h"


void Bullet::update(const bool** map, int width, int height)
{

    if (!explode)
    {

        if (!isMine)
        {
            Vector3D vl = MakeVector(PROJECTILE_BULLET_SPEED, 0, angle);
            float difx = vl.x;
            float dify = vl.y;

            if ((y + radius - dify < height * TILE_WIDTH) && (x + radius - dify < width * TILE_WIDTH) && 
                    (x -radius + difx > -16.f) && (y - radius - difx > -16.f))
            {

                if (tim < 256)
                {

                    int yidx = (int)round((y - (dify * 1.5f)) / TILE_WIDTH);
                    int xidx = (int)round((x + (difx * 1.5f)) / TILE_WIDTH);

                    if ((yidx >= height) || (xidx >= width))
                    {
                        frame = 2;
                        explode = true;
                        return;
                    }

                    if (!map[yidx][xidx])
                    {
                        tim += 2;

                        x +=difx;
                        y -= dify;
                    }
                    else
                    {
                        frame = 2;
                        explode = true;

                    }
                }
                else
                {
                    frame = 2;
                    explode = true;
                }
            }
            else
            {

                frame = 2;
                explode = true;
            }
        }
    }
    else
    {
        explodetim++;

        if (explodetim >= PROJECTILE_EXPLOSION_DURATION)
        {
            explodetim = 0;
            tim = 0;
            exists = false;
            explode = false;
            frame = 0;
        }
    }
}

bool Bullet::onHit(DArray<Dude>& dudes)
{
    int dmg = PROJECTILE_BULLET_DAMAGE;

    if (isMine)
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

    return hit;

}

