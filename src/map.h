#ifndef MAP_H
#define MAP_H


#include "DArray.h"
#include "dude.h"
#include "Decal.h"


class CItem{
public:
 float x;
 float y;
 int value;
};


class CMap 
{
    public:
        char name[20];
        unsigned char width;
        unsigned char height;
        unsigned char** tiles;
        bool** colide;
        DArray<CItem> items;
        DArray<Dude> mons;
        DArray<Decal> decals;

        int timeToComplete; //per kiek laiko reikia pereit
        int misionItems; //kiek bus flopiku,cd,captive
        int goods; //kiek mape bus ammo,medi,timer
        int enemyCount;

        Vector3D start;
        Vector3D exit;

        CMap(){
            tiles=0; width=0; height=0;
            timeToComplete=0;
            misionItems=0;
            goods=0;
            enemyCount=0;
        }

        Dude* getPlayer();

        bool Load(const char* path, bool createItems=true, int otherplayers=0);
        void Destroy();
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
