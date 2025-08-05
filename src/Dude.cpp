#include "Dude.h"

#include <cmath>
#include "Usefull.h"
#include "gui/Text.h"
#include "bullet.h"
#include "BulletContainer.h"
//


//------------------------
void Dude::heal(){
    if (hp < 100)
    {
        if (hp > 90)
        {
            hp = 100;
        }
        else
        {
            hp += 10;
        }
    }
}
//-------------------------
void Dude::initMonsterHP()
{
    hp = MONSTER_BASE_HP + rand()%10;
}
//--------------------------------
bool Dude::damage(int dmg)
{
    hp -= dmg;
    return hp <= 0;
}
//-------------------------------------
bool Dude::isColideWithOthers( DArray<Dude>& chars, int count, float newx, float newy)
{

    for (int i=0;i<count;i++)
    {
        if ((CirclesColide(newx,newy,10.0f,chars[i].x,chars[i].y,10.0f))&&(chars[i].id!=id))
        {

            if ((!chars[i].spawn)&&(!chars[i].shot))
                return true;
        }
    }

    return false;
}
//--------------------------------------
void Dude::respawn()
{

    stim++;

    if (stim%5==0)
    {
        if (frame==weaponCount*4+2) 
        {
            frame=weaponCount*4+3;
        }
        else 
        {
            frame=weaponCount*4+2;
        }
    }

    if (stim>=80) 
    {
        spawn=false;
        stim=0;
        frame=(currentWeapon+1)*4-2;
        alive = true;
    }
}
//--------------------------------------------------
void Dude::rotate(float ang)
{
    angle += ang;

    if (angle >= 6.28f)
    {
        angle = 0;
    }

}


//----------------------------------------------------
//judina heroju, speed-greitis, radius - herojaus bounding circle spindulys, map - zemelapio
//praeinamumu masyvas, dirx - ar juda x asimi, diry - ar juda y asimi
void Dude::move(float walkSpeed,float strifeSpeed, float radius, bool** map, int mapsizex, int mapsizey,
                DArray<Dude>& chars,int charcount, int* dirx, int* diry){

    Vector3D vl = MakeVector(walkSpeed, strifeSpeed, angle);
    vl.normalize();


    float difx = vl.x;
    float dify = vl.y;

    float newposx=x+difx;
    float newposy=y-dify;


    Vector3D p1; 
    Vector3D p2;
    Vector3D p3;
    Vector3D p4;

    Vector3D p5; 
    Vector3D p6;
    Vector3D p7;
    Vector3D p8; 


    p1.x = round((newposx - radius) / 32.0f);

    if (p1.x>mapsizex-1)
    {
        p1.x=mapsizex-1;
    }

    p1.y = round((y - radius) / 32.0f);

    if (p1.y>mapsizey-1)
    {
        p1.y = mapsizey-1;
    }

    p2.x=round((newposx+radius)/32.0f);
    if (p2.x>mapsizex-1)
        p2.x=mapsizex-1;
    p2.y=p1.y;

    p3.x=p1.x;
    p3.y=round((y+radius)/32.0f);
    if (p3.y>mapsizey-1)
        p3.y=mapsizey-1;

    p4.x=p2.x;
    p4.y=p3.y;

//--------

   
    p5.x=round((x-radius)/32.0f);
    if (p5.x>mapsizex-1)
        p5.x=mapsizex-1;
    p5.y=round((newposy-radius)/32.0f);
    if (p5.y>mapsizey-1)
        p5.y=mapsizey-1;

    p6.x=round((x+radius)/32.0f);
    if (p6.x>mapsizex-1)
        p6.x=mapsizex-1;
    p6.y=p5.y;

    p7.x=p5.x;
    p7.y=round((newposy+radius)/32.0f);
    if (p7.y>mapsizey-1)
        p7.y=mapsizey-1;

    p8.x=p6.x;
    p8.y=p7.y;



    if (dirx)
        *dirx=0;
    if (diry)
        *diry=0;

    

    if ((!map[(int)p1.y][(int)p1.x]) &&
        (!map[(int)p2.y][(int)p2.x]) &&
        (!map[(int)p3.y][(int)p3.x]) &&
        (!map[(int)p4.y][(int)p4.x]) && 
        (!isColideWithOthers(chars,charcount,newposx,y)) &&
        ((newposx)<=(mapsizex-1)*32.0f) &&
        ((newposy)<((mapsizey-1)*32.0f)) &&
        ((newposx-radius)>=-radius) &&
        ((y-radius)>=-radius)

       )
    {
        if ((x > newposx)&&(dirx))
        {
            *dirx=1;
        }
        else if ((x<newposx)&&(dirx))
        {
            *dirx=2;
        }

        x = x + difx;

    }

    if ((!map[(int)p5.y][(int)p5.x]) &&
        (!map[(int)p6.y][(int)p6.x]) &&
        (!map[(int)p7.y][(int)p7.x]) &&
        (!map[(int)p8.y][(int)p8.x]) &&
        (!isColideWithOthers(chars,charcount,x,newposy)) &&
        ((newposx)<=(mapsizex-1)*32.0f) &&
        ((newposy)<((mapsizey-1)*32.0f))&&
        ((newposx-radius)>=-radius) &&
        ((newposy-radius)>=-radius))
    {

            if ((y>newposy)&&(diry))
                *diry=1;
            else
                if ((y<newposy)&&(diry))
                    *diry=2;

            y = y - dify;
    }

    if ((*diry>0)||(*dirx>0)){
        tim++;
        if (tim>=30) tim=0;
        frame=tim/10+currentWeapon*4;
    }


}
//--------------------------------------------------------------
void Dude::draw(PicsContainer& pics, unsigned index, float posx, float posy)
{
    pics.draw(index,
              x + posx,
              y + posy,
              frame,
              true,
              1.0f,
              1.0f,
              (angle + M_PI / 2.f) * (180 / M_PI),
              COLOR(r,g,b, 1.f),
              COLOR(r,g,b, 1.f));

    /*char buf[10];
    sprintf(buf, "%d", id);
    WriteText(round(x)-((pskx-scrx) * TILE_WIDTH) + posx,
              round(y)-((psky-scry) * TILE_WIDTH) + posy,
              pics,
              10,
              buf);*/

}
//----------------------------------------------------------
bool Dude::shoot(bool useBullets, bool isMine, CBulletContainer* bulcon)
{

    if ((useBullets) && (ammo<=0))
    {
        return false;
    }


    canAtack = false;
    frame = (currentWeapon+1)*4-1;

    if (useBullets)
    {
        if (ammo <= 0)
        {
            return false;
        }


        Bullet newbul;

        newbul.x = x+(cos(-angle) * 8.0f);
        newbul.y = y+(sin(angle) * 10.0f);
        newbul.parentID=id;
        newbul.tim=0;
        newbul.angle=angle;
        newbul.frame=isMine;
        newbul.exists=true;
        newbul.explode=false;
        newbul.explodetim = 0; 
        newbul.isMine=isMine;


        ammo--;

        bulcon->add(newbul);
    }

    return true;
}
//-------------------------------
int Dude::hitIt(Dude& enemy, float vectorx, float vectory, int damage)
{

    if ((CirclesColide(x+vectorx, y-vectory, 4.0f, enemy.x,enemy.y, 8.0f))
            &&(enemy.id!=id))
    {
        enemy.hp -= damage;
        enemy.hit = true;
        enemy.lastDamagedBy = id;
        return enemy.id;
    }

    return -1;
}

//-------------------------------
void Dude::splatter()
{

    stim++;
    frame = weaponCount*4;
    if (stim > 30)
    {
        frame = weaponCount * 4 + 1;

        if (stim > 50)
        {
            shot=false;
            stim=0;
            spawn=true; 
        }
    }
}
//------------------------------
void Dude::appearInRandomPlace(bool** map, int mapwidth, int mapheight){
    
    startX = rand() % mapwidth;
    startY = rand() % mapheight;

    while (map[startY][startX])
    {
        startX = rand()%mapwidth;
        startY = rand()%mapheight;
    }

    x = startX * 32.0f;
    y = startY * 32.0f;

}
//----------------------------------------
void Dude::damageAnim(){
    g=b=0;
    r=1.0f;
    hittim++;
    if (hittim>=10){
        hittim=0;
        hit=false;
        r=g=b=1.0f;
    }
}
//------------------------------
void Dude::reload(int time){
    reloadtime++;
    if (reloadtime>time/2)
     frame=(currentWeapon+1)*4-2;
    if (reloadtime==time){
        canAtack=true;
        reloadtime=0;
    }

}
//-------------------------------
void Dude::chageNextWeapon(){
    currentWeapon++;
    if (currentWeapon>=weaponCount)
        currentWeapon=0;
    frame=(currentWeapon+1)*4-2;
    
}
