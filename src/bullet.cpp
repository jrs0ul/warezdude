#include <cmath>
#include "bullet.h"
#include "Usefull.h"


void Bullet::ai(bool** map, int width, int height)
{

    if (!explode)
    {

        if (!isMine)
        {
            Vector3D vl = MakeVector(2.0f,0,angle);
            float difx = vl.x;
            float dify = vl.y;

            if ((y - (dify * 1.5f) + radius < height * 32.0f) && (x + (difx * 1.5f) + radius < width * 32.0f) && 
                    (x + (difx * 1.5f) - radius > 0.0f) && (y - (dify * 1.5f) - radius > 0.0f))
            {

                if ((tim < 256)&&
                    (!map[(int)round((y-(dify*1.5f))/32.0f)][(int)round((x+(difx*1.5f))/32.0f)])){

                        tim+=2; 

                        x += difx;
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
    }
    else
    {
        explodetim++;

        if (explodetim >= 10)
        {
            explodetim=0;
            tim=0;
            exists=false;
            explode=false;
            frame=0;
        }
    }
}

