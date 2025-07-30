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
//-------------------------------
void CMap::move(Vector3D v, float size)
{
    mapPos = mapPos + Vector3D(v.x * size, v.y * size , v.z * size);
}


//-----------------------------------------
void CMap::arangeItems(){
    int ix;
    int iy;
    //isdelioja flopikus
    for (int a=0; a<misionItems; a++){
        ix=rand()%_width;
        iy=rand()%_height;

        bool found=false;
        for (unsigned long i=0;i<items.count();i++){
            if ((ix*1.0f==items[i].x)&&(iy*1.0f==items[i].y)){
                found=true;
                break;
         }
        }

        while ((_colide[iy][ix]) || (found) || (tiles[iy][ix]==35)||(tiles[iy][ix]==36)){
            ix=rand() % _width;
            iy=rand() % _height;
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

    //places ammo and medkits
    for (int a=0; a<goods; a++)
    {
        ix=rand() % _width;
        iy=rand() % _height;



        bool found=false;
        for (unsigned long i=0;i<items.count();i++){
            if ((ix*1.0f==items[i].x)&&(iy*1.0f==items[i].y)){
                found=true;
                break;
         }
        }

        while ((_colide[iy][ix]) || (found)){
            ix=rand() % _width;
            iy=rand() % _height;

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
bool CMap::load(const char* path, bool createItems, int otherplayers){

    char buf[255];

    puts(path);

    strcpy(name, path);

    printf("Loading [%s]\n", path);

    enemyCount = 0;

    Xml mapfile;

    bool res = mapfile.load(path);

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
                                        _width = atoi(buf);
                                        printf("MAP WIDTH: %d\n", _width);
                                    }
                                }
                                else if (wcscmp(attr->getName(), L"height") == 0)
                                {
                                    wchar_t* val = attr->getValue();

                                    if (val)
                                    {
                                        sprintf(buf, "%ls", val);
                                        _height = atoi(buf);
                                        tiles  = new unsigned char*[_height];
                                        _colide = new bool* [_height];
                                        printf("MAP HEIGHT: %d\n", _height);
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

                                tiles[a] = new unsigned char[_width];
                                _colide[a] = new bool[_width];

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
                                            _colide[a][j] = true;
                                        }
                                        else
                                        {
                                            _colide[a][j]=false;
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
                                naujas.race=rand() % MONSTER_MAX_RACE + 1;
                                naujas.initMonsterHP();
                                mons.add(naujas);

                            }

                        }
                    }

                }
            }
        }
    }


    mapfile.destroy();

    printf("map width %d, height %d\n", _width, _height);

    start.x = start.x * 32;
    start.y = start.y * 32;

    if ((enemyCount + 1 + otherplayers) != 0)
    {

        Dude playeris;
        playeris.id = 254;
        playeris.weaponCount=3;
        playeris.currentWeapon=1;
        playeris.frame = (playeris.currentWeapon+1)*4-2;
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
void CMap::draw(PicsContainer& pics, float r, float g, float b, int pskx, int psky, int scrx, int scry, int posx, int posy, int pushx, int pushy)
{
    int tmpy = 0;
    int tmpx = 0;

    unsigned tileset = pics.findByName("pics/tileset.tga");


    for (int a = psky - scry; a < psky; ++a)
    {
        tmpx = 0;

        for (int i = pskx - scrx; i < pskx; ++i)
        {
            if (tiles)
            {
                if (tiles[a])
                {
                    pics.draw(tileset,
                              32 * tmpx + pushx - posx,
                              32 * tmpy + pushy - posy,
                              tiles[a][i] - 1,
                              true,
                              1.f, 1.f, 0.f, COLOR(r,g,b, 1.0f), COLOR(r, g, b, 1.0f));
                }
            }

            tmpx++;
        }

        tmpy++;

    }

}


//--------------------------------------
void CMap::destroy()
{

    mons.destroy();

    if (tiles)
    {
        for (unsigned a=0; a < _height; a++)
        {
            delete []tiles[a];
        }

        delete []tiles;
        tiles=0;
    }

    if (_colide)
    {
        for (unsigned a=0; a < _height; a++)
        {
            delete []_colide[a];
        }

        delete []_colide;
        _colide=0;
    }

    items.destroy();
    decals.destroy();


}
//-------------------------------------------
bool CMap::colide(unsigned x, unsigned y)
{

    if (!_colide)
    {
        return false;
    }

    return _colide[y][x];

}

//-------------------------------------------


void CMap::ReplaceTiles(unsigned char old, unsigned char fresh){
 for (unsigned a = 0; a < _height; a++)
  for (unsigned i=0;i < _width;i++){
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
