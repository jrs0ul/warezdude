#ifndef PICTURE_H
#define PICTURE_H

/*
jrs0ul
2007 02 12
*/

#include <d3d9.h>
#include <d3dx9.h>


class Picture{
  public:	
	LPDIRECT3DTEXTURE9 image;
	D3DXIMAGE_INFO info;
	
	unsigned tilewidth;
	unsigned tileheight;
	int vframes,hframes;
	RECT prect;

	Picture();
	void Clear();
	void SetTileSize(int dx, int dy);
	void Blt(LPD3DXSPRITE& spraitas, int x, int y, int frame=0,float alpha=1.0f,
		   float scalex=1.0f, float scaley=1.0f,float angle=0.0f,float r=1.0,float g=1.0,float b=1.0,bool center=false );
	bool Load(LPDIRECT3DDEVICE9& D3DDevice, const char* pic);
	int TotalFrames();
};

//----------------------------
int AvailSysMem(void);
//------------------------------


class PictureContainer{
public:
 Picture * images;
 int count;

 struct PicData{
   char name[255];
   unsigned twidth;
   unsigned theight;
  }* list;

 PictureContainer(){images=0; list=0; count=0;}

 bool initContainer(const char* path);

 bool loadpicture(LPDIRECT3DDEVICE9& D3DDevice,int index);

 Picture* findpic(const char* name);

 void deleteContainer(){
  if (list)
   delete []list;
  if (images)
   delete []images;
  list=0; 
  count=0;
 }

};

#endif // PICTURE_H
//===================================================
