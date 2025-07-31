#ifndef MAP_H
#define MAP_H


#include "DArray.h"
#include "Dude.h"
#include "Decal.h"


class CItem{
public:
 float x;
 float y;
 int value;
};

enum Items
{
    ITEM_CD = 1,
    ITEM_FLOPPY = 2,
    ITEM_CARTRIDGE = 3,
    ITEM_EXIT = 4,
    ITEM_AMMO_PACK = 5,
    ITEM_MEDKIT
};


class CMap 
{
        unsigned _width;
        unsigned _height;
    public:
        char name[20];
        unsigned char** tiles;
        bool** _colide;
        DArray<CItem> items;
        DArray<Dude> mons;
        DArray<Decal> decals;
        int itmframe;

        int timeToComplete;
        int misionItems;
        int goods; // ammo, medkits, etc.
        int enemyCount;

        Vector3D mapPos;

        Vector3D start;
        Vector3D exit;

        CMap() : _width(0), _height(0){
            tiles=0;
            timeToComplete=0;
            misionItems=0;
            goods=0;
            enemyCount=0;
        }

        Dude* getPlayer();

        unsigned width(){return _width;}
        unsigned height(){return _height;}
        void setWidth(unsigned newWidth){_width = newWidth;}
        void setHeight(unsigned newHeight){_height = newHeight;}

        Vector3D getPos(){return mapPos;}
        void move(Vector3D v, float size);

        bool load(const char* path, bool createItems=true, int otherplayers=0);
        bool save(const char* path);
        void draw(PicsContainer& pics, float r, float g, float b, int pskx, int psky, int scrx, int scry, int posx, int posy, int pushx, int pushy);
        void destroy();

        bool colide(unsigned x, unsigned y);
        void ReplaceTiles(unsigned char old, unsigned char fresh);
        void addItem(float nx, float ny, int nvalue);
        void removeItem(int ID);
        void addMonster(Dude& newmonster);
        void removeMonster(int index);
        void fadeDecals();

    private:
        void arangeItems();

};




#endif //MAP_H
