#include "Decal.h"
#include "Usefull.h"
#include "Consts.h"

void Decal::draw(PicsContainer& pics, unsigned index,
                int posx, int posy)
{

    pics.draw(index, x + posx, y + posy,
              frame,
              true,
              1.0f,1.0f,0,
              COLOR(r,g,b, alpha),
              COLOR(r, g, b, alpha)); 
}
