#include <cstdio>
#include "map.h"


//-----------------------------------------
void CMap::addMonster(Dude &newmonster, bool isPlayer){
    mons.add(newmonster);
}
//-------------------------------------------
void CMap::removeMonster(int index){
    mons.remove(index);
}

//-----------------------------------------
void CMap::arangeItems(){
    int ix;
    int iy;
    //isdelioja flopikus
    for (int a=0; a<misionItems; a++){
        ix=rand()%width;
        iy=rand()%height;

        bool found=false;
        for (unsigned long i=0;i<items.count();i++){
            if ((ix*1.0f==items[i].x)&&(iy*1.0f==items[i].y)){
                found=true;
                break;
         }
        }

        while ((colide[iy][ix])||(found)||(tiles[iy][ix]==35)||(tiles[iy][ix]==36)){
            ix=rand()%width;
            iy=rand()%height;
            found=false;
            for (unsigned long i=0;i<items.count();i++){
                if ((ix*1.0f==items[i].x)&&(iy*1.0f==items[i].y)){
                    found=true;
                    break;
                }
            }
        }

        addItem(ix*32.0f,iy*32.0f,rand()%3+1);

    }

    //isdelioja ammo, medi, timers
    for (int a=0; a<goods; a++){
        ix=rand()%width;
        iy=rand()%height;



        bool found=false;
        for (unsigned long i=0;i<items.count();i++){
            if ((ix*1.0f==items[i].x)&&(iy*1.0f==items[i].y)){
                found=true;
                break;
         }
        }

        while ((colide[iy][ix])||(found)){
            ix=rand()%width;
            iy=rand()%height;

            found=false;
            for (unsigned long i=0;i<items.count();i++){
                if ((ix*1.0f==items[i].x)&&(iy*1.0f==items[i].y)){
                    found=true;
                    break;
                }
            }
        }

        if ((rand()%2+1)==2)
            addItem(ix*32.0f,iy*32.0f,5);
        else
            addItem(ix*32.0f,iy*32.0f,6);

    }



}


//-------------------------------------
bool CMap::Load(const char* path, bool createItems, int otherplayers){

    FILE* mapas;
    int c=0;
    char bufas[255];

    strcpy(bufas,"maps/");

    strcat(bufas,path);
    strcpy(name,path);

    mapas=fopen(bufas,"rt");

    if (!mapas)
        return false;

    fscanf(mapas,"%u %u",&width,&height);


    tiles  = new unsigned char*[height];
    colide = new bool* [height];

    fgetc(mapas); //readln
    for (int a=0; a<height; a++){
        tiles[a]=new unsigned char[width];
        colide[a]=new bool[width];
        for (int i=0; i<width; i++){
            c=fgetc(mapas);
            tiles[a][i]=c-48;
            if (((tiles[a][i]>=19)&&(tiles[a][i]<=34))||((tiles[a][i]>=44)&&(tiles[a][i]<48))||
                ((tiles[a][i]>=50)&&(tiles[a][i]<=65))||(tiles[a][i]==67)||(tiles[a][i]==69)||(tiles[a][i]==71)
                ||(tiles[a][i]==9))
                colide[a][i]=true;
            else colide[a][i]=false;
        }
        fgetc(mapas);
    }



    //cia pradeda zaidejas
    fscanf(mapas,"%d %d",&start.x,&start.y);
    fscanf(mapas,"%d %d",&exit.x,&exit.y);
    start.x=start.x*32;
    start.y=start.y*32;
    fscanf(mapas,"%d",&misionItems);
    fscanf(mapas,"%d",&timeToComplete);
    fscanf(mapas,"%d",&goods);
    //kiek monstru;
    enemyCount=0;
    fscanf(mapas,"%d",&enemyCount);
    if ((enemyCount+1+otherplayers)!=0){
        //mons=new Dude[enemyCount+1+otherplayers];

        for (int i=0; i<enemyCount; i++){
            int sx,sy;
            fscanf(mapas,"%d %d",&sx, &sy);
            Dude naujas;
            naujas.id=i;
            naujas.start.x=sx*32;
            naujas.start.y=sy*32;
            naujas.x=(float)naujas.start.x;
            naujas.y=(float)naujas.start.y;
            naujas.race=rand()%3+1; 
            naujas.hp=30+rand()%10;
            mons.add(naujas);
        }
        Dude playeris;
        playeris.id=254;
        playeris.hp=100;
        playeris.weaponCount=3;
        playeris.currentWeapon=1;
        playeris.frame=(playeris.currentWeapon+1)*4-2;  
        mons.add(playeris);
        for (int i=0;i<otherplayers;i++){
         playeris.id++;
         mons.add(playeris);
        }
    }

    fclose(mapas);


    if (createItems)
        arangeItems();


    return true;
}
//--------------------------------------
void CMap::Destroy(){

 mons.destroy();

 if (tiles){
  for (int a=0;a<height;a++)
    delete []tiles[a];
  delete []tiles;
  tiles=0;
 } 
 if (colide){
  for (int a=0;a<height;a++)
    delete []colide[a];
  delete []colide;
  colide=0;
 }

 items.destroy();
 decals.destroy();
 

}
//-------------------------------------------


void CMap::ReplaceTiles(unsigned char old, unsigned char fresh){
 for (int a=0;a<height;a++)
  for (int i=0;i<width;i++){
    if (tiles[a][i]==old)
        tiles[a][i]=fresh;
  }
}

//-------------------------------------------
void CMap::addItem(float nx, float ny, int nvalue){
    CItem newitem;
    newitem.x=nx;
    newitem.y=ny;
    newitem.value=nvalue;
    items.add(newitem);

}

//-------------------------------------------
void CMap::removeItem(int ID){
    items.remove(ID);

}

//-------------------------------------
void CMap::fadeDecals(){
    for (unsigned int i=0;i<decals.count();i++){
        decals[i].alpha-=0.0005f;
        if (decals[i].alpha<=0.0f)
            decals.remove(i);
    }
        
}
