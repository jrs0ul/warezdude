#include "Decal.h"
#include "Usefull.h"

void Decal::draw(Picture& sprite, Renderer3D& rend,
				int pskx, int scrx, int psky, int scry,
				int pushx, int posx, int pushy, int posy){

					sprite.Blt(rend.spraitas,round(x)-((pskx-scrx)*32)+pushx-posx,
						round(y)-((psky-scry)*32)+pushy-posy,
						frame,alpha,1.0f,1.0f,0,
						r,g,b,true); 
}