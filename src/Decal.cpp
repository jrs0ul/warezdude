#include "Decal.h"
#include "Usefull.h"

void Decal::draw(PicsContainer& pics, unsigned index,
                int pskx, int scrx, int psky, int scry,
                int pushx, int posx, int pushy, int posy)
{

    pics.draw(index,
              round(x)-((pskx-scrx)*32)+pushx-posx,
              round(y)-((psky-scry)*32)+pushy-posy,
              frame,
              true,
              1.0f,1.0f,0,
              COLOR(r,g,b, alpha),
              COLOR(r, g, b, alpha)); 
}
