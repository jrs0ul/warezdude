#ifndef DUDE_H
#define DUDE_H

#include "DArray.h"
#include "Vectors.h"
#include "Consts.h"

class CBulletContainer;
class PicsContainer;

class Dude
{
        int hp;
        bool alive;
public:
        float x;
        float y;
        float angle;
        float r,g,b;

        int item;
        int atim;
        int delay;

        int victimID;

        int id;
        int tim;
        int stim;

        int race;

        int ammo;
        int reloadtime;
        int startX;
        int startY;

        int hittim;

        int weaponCount; //kiek gali tureti ginklu
        int currentWeapon; //koks dabar ginklas naudojamas

        int lastDamagedBy;
        unsigned char colid;
        unsigned char frame;
        bool enemyseen;
        bool hit;
        bool canAtack;
        bool spawn;
        bool shot;



        Dude(float dx = 0, float dy = 0) :
            hp(ENTITY_INITIAL_HP),
            x(dx),
            y(dy),
            angle(0.f),
            shot(false)
        {
            frame=0; tim=0; // dir=0;
            stim=0;
            ammo = ENTITY_INITIAL_AMMO;
            alive=true;
            canAtack=true;
            reloadtime=0;
            r = g = b = 1.0f;
            hit=false;
            hittim=0;
            id=0;

            victimID=0;
            colid=0;
            atim=0;
            enemyseen=false;
            item=0;
            delay = rand()%2+1;
            alive=true;

            spawn=false;
            weaponCount=1;
            currentWeapon=0;
        }


        //ar kolidina su kokiu nors monstru; count - kiek monstru
        bool isColideWithOthers(DArray<Dude>& chars, int count, float newx, float newy);
        void rotate(float angle);
        bool move(float walkSpeed,float strifeSpeed,float radius, const bool** map, int mapsizex, int mapsizey,
                DArray<Dude>& chars, int charcount);
        bool moveGamePad(const Vector3D& movementDir,float radius, const bool** map, int mapsizex, int mapsizey,
                DArray<Dude>& chars, int charcount);
        void respawn();
        //nupaiso
        void draw(PicsContainer& pics, unsigned index, float posx, float posy, int ScreenWidth, int ScreenHeight);

        /* shoots or deploys a mine if the entity has some ammo */
        bool shoot(bool useBullets, bool isMine, CBulletContainer* bulcon);
        //atakuoja, jei kolidina tai true jei ne false
        int hitIt(Dude& enemy, float vectorx, float vetory, int damage);

        bool isAlive(){return alive;}
        //gydosi
        void heal();

        void disintegrationAnimation();
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

private:
        bool movement(Vector3D dir,
                      float radius,
                      const bool** map,
                      int mapsizex,
                      int mapsizey,
                      DArray<Dude>& chars,
                      int charcount);
};


//-------



#endif //DUDE_H
