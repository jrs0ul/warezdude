#include <cmath>
#include "bullet.h"
#include "Usefull.h"
#include "Consts.h"


void Bullet::ai(bool** map, int width, int height)
{

    if (!explode)
    {

        if (!isMine)
        {
            Vector3D vl = MakeVector(2.0f,0,angle);
            float difx = vl.x;
            float dify = vl.y;

            if ((y + radius - dify < height * 32.0f) && (x + radius - dify < width * 32.0f) && 
                    (x -radius + difx > -16.f) && (y - radius - difx > -16.f))
            {

                if (tim<256)
                {


                    int yidx = (int)round((y-(dify*1.5f))/32.0f);
                    int xidx = (int)round((x+(difx*1.5f))/32.0f);

                    if ((yidx >= height) || (xidx >= width))
                    {
                        frame=2;
                        explode=true;
                        return;
                    }

                    if (!map[yidx][xidx])
                    {

                        tim+=2; 

                        x +=difx;
                        y -= dify;
                    }
                    else
                    {
                        frame=2;
                        explode=true;

                    }
                }
                else
                {
                    frame=2;
                    explode=true;
                }
            }
            else
            {

                frame=2;
                explode=true;
            }
        }
    }
    else
    {
        explodetim++;

        if (explodetim >= PROJECTILE_EXPLOSION_DURATION)
        {
            explodetim=0;
            tim=0;
            exists=false;
            explode=false;
            frame=0;
        }
    }
}

