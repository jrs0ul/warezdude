#include <cstdio>
#include "map.h"
#include "Xml.h"




Dude* CMap::getPlayer()
{
    return &mons[enemyCount];
}
//-----------------------------------------
void CMap::addMonster(Dude &newmonster){
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

    char buf[255];

    puts(path);

    strcpy(buf,"maps/");

    strcat(buf,path);
    strcpy(name,path);

    printf("Loading [%s]\n", buf);

    enemyCount = 0;

    Xml mapfile;

    bool res = mapfile.load(buf);

    if (res)
    {
        XmlNode* mainnode = mapfile.root.getNode(L"Map");


        if (mainnode)
        {
            for (unsigned i = 0; i < mainnode->childrenCount(); ++i)
            {
                XmlNode* node = mainnode->getNode(i);

                if (node)
                {
                    if (wcscmp(node->getName(), L"size") == 0)  //  parse size
                    {
                        for (unsigned a = 0; a < node->attributeCount(); ++a)
                        {
                            XmlAttribute *attr = node->getAttribute(a);

                            if (attr)
                            {
                                if (wcscmp(attr->getName(), L"width") == 0)
                                {
                                    wchar_t* val = attr->getValue();

                                    if (val)
                                    {
                                        sprintf(buf, "%ls", val);
                                        width = atoi(buf);
                                        printf("MAP WIDTH: %d\n", width);
                                    }
                                }
                                else if (wcscmp(attr->getName(), L"height") == 0)
                                {
                                    wchar_t* val = attr->getValue();

                                    if (val)
                                    {
                                        sprintf(buf, "%ls", val);
                                        height = atoi(buf);
                                        tiles  = new unsigned char*[height];
                                        colide = new bool* [height];
                                        printf("MAP HEIGHT: %d\n", height);
                                    }

                                }
                            }
                        }
                    }
                    else if (wcscmp(node->getName(), L"rows") == 0)  //  parse rows
                    {
                        for (unsigned a = 0; a < node->childrenCount(); ++a)
                        {
                            XmlNode* row = node->getNode(a);

                            if (row)
                            {

                                tiles[a] = new unsigned char[width];
                                colide[a] = new bool[width];

                                for (unsigned j = 0; j < row->childrenCount(); ++j)
                                {
                                    XmlNode* tile = row->getNode(j);

                                    if (tile)
                                    {
                                        sprintf(buf, "%ls", tile->getValue());
                                        tiles[a][j] = atoi(buf) - 48;

                                        if (((tiles[a][j]>=19)&&(tiles[a][j]<=34))||((tiles[a][j]>=44)&&(tiles[a][j]<48))||
                                                ((tiles[a][j]>=50)&&(tiles[a][j]<=65))||(tiles[a][j]==67)||(tiles[a][j]==69)||(tiles[a][j]==71)
                                                ||(tiles[a][j]==9))
                                        {
                                            colide[a][j]=true;
                                        }
                                        else
                                        {
                                            colide[a][j]=false;
                                        }

                                    }

                                }
                            }
                        }
                    }
                    else if (wcscmp(node->getName(), L"start") == 0) // start pos
                    {
                        for (unsigned a = 0; a < node->attributeCount(); ++a)
                        {
                            XmlAttribute *attr = node->getAttribute(a);

                            if (attr)
                            {
                                if (wcscmp(attr->getName(), L"x") == 0)
                                {
                                    wchar_t* val = attr->getValue();

                                    if (val)
                                    {
                                        sprintf(buf, "%ls", val);
                                        start.x = atoi(buf);
                                        printf("PLAYER START.X: %f\n", start.x);
                                    }
                                }
                                else if (wcscmp(attr->getName(), L"y") == 0)
                                {
                                    wchar_t* val = attr->getValue();

                                    if (val)
                                    {
                                        sprintf(buf, "%ls", val);
                                        start.y = atoi(buf);
                                        printf("PLAYER START.Y: %f\n", start.y);
                                    }

                                }
                            }
                        }
                    }
                    else if (wcscmp(node->getName(), L"exit") == 0) // exit pos
                    {
                        for (unsigned a = 0; a < node->attributeCount(); ++a)
                        {
                            XmlAttribute *attr = node->getAttribute(a);

                            if (attr)
                            {
                                if (wcscmp(attr->getName(), L"x") == 0)
                                {
                                    wchar_t* val = attr->getValue();

                                    if (val)
                                    {
                                        sprintf(buf, "%ls", val);
                                        exit.x = atoi(buf);
                                        printf("PLAYER EXIT.X: %f\n", exit.x);
                                    }
                                }
                                else if (wcscmp(attr->getName(), L"y") == 0)
                                {
                                    wchar_t* val = attr->getValue();

                                    if (val)
                                    {
                                        sprintf(buf, "%ls", val);
                                        exit.y = atoi(buf);
                                        printf("PLAYER EXIT.Y: %f\n", exit.y);
                                    }

                                }
                            }
                        }
                    }
                    else if (wcscmp(node->getName(), L"items") == 0) // key item count
                    {
                        sprintf(buf, "%ls", node->getValue());
                        misionItems = atoi(buf);
                    }
                    else if (wcscmp(node->getName(), L"time") == 0) // time to complete
                    {
                        sprintf(buf, "%ls", node->getValue());
                        timeToComplete = atoi(buf);
                    }
                    else if (wcscmp(node->getName(), L"goods") == 0) // goodies
                    {
                        sprintf(buf, "%ls", node->getValue());
                        goods = atoi(buf);
                    }
                    else if (wcscmp(node->getName(), L"enemies") == 0) // goodies
                    {
                        enemyCount = node->childrenCount();

                        for (unsigned a = 0; a < node->childrenCount(); ++a)
                        {
                            XmlNode* enemy = node->getNode(a);

                            if (enemy)
                            {

                                int sx,sy;
                                Dude naujas;
                                naujas.id = a;

                                for (unsigned a = 0; a < enemy->attributeCount(); ++a)
                                {
                                    XmlAttribute *attr = enemy->getAttribute(a);

                                    if (attr)
                                    {
                                        if (wcscmp(attr->getName(), L"x") == 0)
                                        {
                                            wchar_t* val = attr->getValue();

                                            if (val)
                                            {
                                                sprintf(buf, "%ls", val);
                                                sx = atoi(buf);
                                            }
                                        }
                                        else if (wcscmp(attr->getName(), L"y") == 0)
                                        {
                                            wchar_t* val = attr->getValue();

                                            if (val)
                                            {
                                                sprintf(buf, "%ls", val);
                                                sy = atoi(buf);
                                            }

                                        }
                                    }
                                } // for

                                naujas.startX = sx*32;
                                naujas.startY = sy*32;
                                naujas.x=(float)naujas.startX;
                                naujas.y=(float)naujas.startY;
                                naujas.race=rand()%3+1; 
                                naujas.hp=30+rand()%10;
                                mons.add(naujas);

                            }

                        }
                    }

                }
            }
        }
    }


    mapfile.destroy();

    printf("map width %d, height %d\n", width, height);

    start.x = start.x * 32;
    start.y = start.y * 32;

    if ((enemyCount + 1 + otherplayers) != 0)
    {

        Dude playeris;
        playeris.id=254;
        playeris.hp=100;
        playeris.weaponCount=3;
        playeris.currentWeapon=1;
        playeris.frame=(playeris.currentWeapon+1)*4-2;
        mons.add(playeris);

        for (int i=0;i<otherplayers;i++)
        {
            playeris.id++;
            mons.add(playeris);
        }
    }



    if (createItems)
    {
        arangeItems();
    }


    return true;
}
//--------------------------------------
void CMap::Destroy()
{

    mons.destroy();

    if (tiles)
    {
        for (int a=0;a<height;a++)
        {
            delete []tiles[a];
        }

        delete []tiles;
        tiles=0;
    }

    if (colide)
    {
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
