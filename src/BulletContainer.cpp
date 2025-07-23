#include <cstring>
#include "Consts.h"
#include "BulletContainer.h"

void CBulletContainer::add(Bullet& newbulet){

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

void CBulletContainer::draw(PicsContainer& pics, int pskx, int psky, int pushx, int pushy, int scrx, int scry, int posx, int posy)
{
    for (int z = 0; z < _count; z++)
    {
        pics.draw(6,
                round(buls[z].x)-((pskx-scrx) * TILE_WIDTH)+pushx-posx,
                round(buls[z].y)-((psky-scry) * TILE_WIDTH)+pushy-posy,
                buls[z].frame,
                true,
                1.0f,1.0f,
                (buls[z].angle + (3.14f/2.0f)) * (180/M_PI)
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
