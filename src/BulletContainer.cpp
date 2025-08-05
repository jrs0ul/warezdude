#include "BulletContainer.h"
#include <cstring>
#include "Consts.h"
#include "bullet.h"
#include "TextureLoader.h"

void CBulletContainer::add(Bullet& newbulet)
{

    Bullet* tmp=0;
    if (buls){
        
        tmp=new Bullet[_count];
        
        memcpy(tmp,buls,sizeof(Bullet)*_count);
        delete []buls;
    }


        _count++;
        buls = new Bullet[_count];

    if (buls){
        memcpy(buls,tmp,sizeof(Bullet)*(_count-1));
        delete []tmp;
    }
    
    buls[_count-1]=newbulet;

    

}
//---------------------------
 void CBulletContainer::removeDead(){
     if (buls){
      int inactive=0;
     
      for (int i=0;i<_count;i++){
      
       if (!buls[i].exists)
        inactive++;
      }
    
      
      if (inactive){
      // fprintf(fil,"destroying inactive...\n");
       int index;
       for (int i=0;i<inactive;i++){
        index=0;
        while ((buls[index].exists)&&(index<_count))
         index++;
        for (int i=index;i<_count-1;i++)
         buls[i]=buls[i+1];

       }//for
      
       Bullet* tmp;
       _count-=inactive;
       if (_count>0){

        
        tmp=new Bullet[_count];
        memcpy(tmp,buls,sizeof(Bullet)*_count);
        delete []buls;
        buls = new Bullet[_count];
        memcpy(buls,tmp,sizeof(Bullet)*_count);
        delete []tmp;

     
       }
       else{
        delete []buls;
        buls=0;
       }
      }
      
      
     }
     
 }
//---------------------------

void CBulletContainer::draw(PicsContainer& pics, float posx, float posy, int ScreenWidth, int ScreenHeight)
{
    for (int z = 0; z < _count; z++)
    {
        const float bulletX = buls[z].x + posx;
        const float bulletY = buls[z].y + posy;

        if (bulletX + HALF_TILE_WIDTH < 0 || bulletX - HALF_TILE_WIDTH > ScreenWidth ||
            bulletY + HALF_TILE_WIDTH < 0 || bulletY - HALF_TILE_WIDTH > ScreenHeight)
        {
            continue;
        }


        pics.draw(6,
                bulletX,
                bulletY,
                buls[z].frame,
                true,
                1.0f,
                1.0f,
                (buls[z].angle + (M_PI / 2.0f)) * (180 / M_PI)
                ); 
    }

}


//---------------------------

 void CBulletContainer::destroy(){
     if (buls){
      delete []buls;
      buls=0;
     }
     _count=0; 
     
 }
