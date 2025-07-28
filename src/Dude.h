#ifndef DUDE_H
#define DUDE_H

#include "DArray.h"
#include "BulletContainer.h"
#include "TextureLoader.h"
#include "Vectors.h"
#include "Consts.h"



class Dude
{
        bool alive;
        int hp;
    public:
        float x,y;
        float angle;

        int item;
        unsigned char colid;
        int atim;

        bool enemyseen;

        int greitis; 
        int delay;

        int victimID;

        int id;
        unsigned char frame;
        int tim;
        int stim;

        int race;

        bool shot;
        bool spawn;
        int ammo;
        bool canAtack;
        int reloadtime;
        int startX;
        int startY;

        float r,g,b;
        bool hit;
        int hittim;

        int weaponCount; //kiek gali tureti ginklu
        int currentWeapon; //koks dabar ginklas naudojamas

        int lastDamagedBy;



        Dude(int dx=0,int dy=0):x((float)dx), y((float)dy){
            frame=0; tim=0; // dir=0;
            angle=0;
            shot=false;
            stim=0;
            ammo = ENTITY_INITIAL_AMMO;
            alive=true;
            canAtack=true;
            reloadtime=0;
            hp = ENTITY_INITIAL_HP;
            r = g = b = 1.0f;
            hit=false;
            hittim=0;
            id=254;

            victimID=0;
            colid=0;
            atim=0;
            enemyseen=false;
            item=0;
            greitis=0;
            delay = rand()%2+1;
            alive=true;

            spawn=false;
            weaponCount=1;
            currentWeapon=0;
        }


        //ar kolidina su kokiu nors monstru; count - kiek monstru
        bool isColideWithOthers(DArray<Dude>& chars, int count, float newx, float newy);
        void rotate(float angle);
        void move(float walkSpeed,float strifeSpeed,float radius, bool** map, int mapsizex, int mapsizey,
                DArray<Dude>& chars,int charcount, int* dirx, int* diry);
        void respawn();
        //nupaiso
        void draw(PicsContainer& pics, unsigned index,
                int pskx, int scrx, int psky, int scry,
                int pushx, int posx, int pushy, int posy);

        /* shoots or deploys a mine if the entity has some ammo */
        bool shoot(bool useBullets, bool isMine, CBulletContainer* bulcon);
        //atakuoja, jei kolidina tai true jei ne false
        int hitIt(Dude& enemy, float vectorx, float vetory, int damage);

        bool isAlive(){return alive;}
        //gydosi
        void heal();
        //isitasko
        void splatter();
        void appearInRandomPlace(bool** map, int mapwidth, int mapheight);
        //kai kas nors atakuoja - paraudonuoja
        void damageAnim();
        //prasuka laika, kad vel galetu atakuoti
        void reload(int time);
        void chageNextWeapon();

        int getHP(){return hp;}
        void initMonsterHP();
        void setHP(int newHP){hp = newHP;}
        //true if hp <= 0
        bool damage(int dmg);
};


//-------



#endif //DUDE_H
