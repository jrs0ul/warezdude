#include <cstdio>
#include <wchar.h>
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
void GenerationDivide(BSPTreeNode* parent)
{
    parent->left = new BSPTreeNode;
    parent->right = new BSPTreeNode;


    if (rand() % 2 == 1) //vertical split
    {
        printf("VERTICAL\n");
        parent->left->startx = parent->startx;
        parent->left->starty = parent->starty;
        parent->left->height = 4 + (rand() % (parent->height / 3));
        parent->left->width = parent->width;

        parent->right->startx = parent->left->startx;
        parent->right->starty = parent->left->starty + parent->left->height;
        parent->right->width = parent->width;
        parent->right->height = parent->height - parent->left->height;
    }
    else
    {
        printf("HORIZONTAL\n");
        parent->left->startx = parent->startx;
        parent->left->starty = parent->starty;
        parent->left->height = parent->height;
        parent->left->width = 4 + (rand() % (parent->width / 3));

        parent->right->startx = parent->startx + parent->left->width;
        parent->right->starty = parent->starty;
        parent->right->width = parent->width - parent->left->width;
        parent->right->height = parent->height;
    }

    if (parent->left->width >= 8 && parent->left->height >= 8)
    {
       GenerationDivide(parent->left);
    }

    if (parent->right->width >= 8 && parent->right->height >= 8)
    {
        GenerationDivide(parent->right);
    }
}

//-------------------------------------
void Erode(BSPTreeNode* parent, CMap* map)
{
    const unsigned char DIRT = 1;

    if (parent->left == nullptr && parent->right == nullptr)
    {
        for (int i = parent->starty + 1; i < parent->starty + parent->height - 1; ++i)
        {
            for (int a = parent->startx + 1; a < parent->startx + parent->width - 1; ++a)
            {
                //printf("[%d, %d] ", i, a);
                map->tiles[i][a] = DIRT;
            }
            //printf("\n");
        }
        //printf("\n\n");

        printf("Eroding %d %d %d %d\n", parent->startx, parent->starty, parent->width, parent->height);

        return;
    }
    else
    {
        Erode(parent->left, map);
        Erode(parent->right, map);
    }
}


//-------------------------------------
void CMap::generate()
{

    const unsigned char WALL = 19;


    BSPTreeNode root;
    root.startx = 0;
    root.starty = 0;
    root.width = _width;
    root.height = _height;

    GenerationDivide(&root);

    for (unsigned i = 0; i < _height; ++i)
    {
        for (unsigned a = 0; a < _width; ++a)
        {
            tiles[i][a] = WALL;
        }
    }

    Erode(&root, this);
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

                                int sx = 0;
                                int sy = 0;
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
bool CMap::save(const char* path)
{
    printf("SAVING %s...\n", path);

    Xml mapfile;

    XmlNode mapnode;
    mapnode.setName(L"Map");
    XmlNode size;
    size.setName(L"size");
    wchar_t width[10];
    swprintf(width, 10, L"%d", _width);
    size.addAtribute(L"width", width);
    wchar_t height[10];
    swprintf(height, 10, L"%d", _height);
    size.addAtribute(L"height", height);
    mapnode.addChild(size);

    XmlNode rows;
    rows.setName(L"rows");

    for (unsigned i = 0; i < _height; ++i)
    {
        XmlNode row;
        row.setName(L"row");

        for (unsigned a = 0; a < _width; ++a)
        {
            XmlNode tile;
            tile.setName(L"t");
            wchar_t value[10];
            swprintf(value, 10, L"%d", tiles[i][a] + 48);
            tile.setValue(value);
            row.addChild(tile);
        }

        rows.addChild(row);
    }
    mapnode.addChild(rows);


    XmlNode startTag;
    startTag.setName(L"start");
    wchar_t buffer[50];
    printf("%d %d\n", (int)start.x/32, (int)start.y/32);
    swprintf(buffer, 50, L"%d", (int)start.x / 32);
    startTag.addAtribute(L"x", buffer);

    swprintf(buffer, 50, L"%d", (int)start.y / 32);
    startTag.addAtribute(L"y", buffer);
    mapnode.addChild(startTag);

    XmlNode exitTag;
    exitTag.setName(L"exit");
    swprintf(buffer, 50, L"%d", (int)exit.x);
    exitTag.addAtribute(L"x", buffer);
    swprintf(buffer, 50, L"%d", (int)exit.y);
    exitTag.addAtribute(L"y", buffer);
    mapnode.addChild(exitTag);

    XmlNode questItemTag;
    questItemTag.setName(L"items");
    swprintf(buffer, 50, L"%d", misionItems);
    questItemTag.setValue(buffer);
    mapnode.addChild(questItemTag);

    XmlNode timeTag;
    timeTag.setName(L"time");
    swprintf(buffer, 50, L"%d", timeToComplete);
    timeTag.setValue(buffer);
    mapnode.addChild(timeTag);

    XmlNode itemsTag;
    itemsTag.setName(L"goods");
    swprintf(buffer, 50, L"%d", goods);
    itemsTag.setValue(buffer);
    mapnode.addChild(itemsTag);

    XmlNode enemies;
    enemies.setName(L"enemies");

    for (unsigned i = 0; i < (unsigned)enemyCount; ++i)
    {
        XmlNode enemyPos;
        enemyPos.setName(L"pos");
        wchar_t buf[10];
        swprintf(buf, 10, L"%d", (int)mons[i].x / 32);
        enemyPos.addAtribute(L"x", buf);
        swprintf(buf, 10, L"%d", (int)mons[i].y / 32);
        enemyPos.addAtribute(L"y", buf);
        enemies.addChild(enemyPos);
    }

    mapnode.addChild(enemies);

    mapfile.root.addChild(mapnode);
    mapfile.write(path);

    mapfile.destroy();

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


    for (unsigned i = 0; i < decals.count(); i++)
    {
        decals[i].draw(pics, 15, pskx,scrx,psky,scry,pushx,posx,pushy,posy);
    }


    for (unsigned long i = 0; i < items.count(); ++i)
    {

        if (items[i].value < ITEM_AMMO_PACK)
        {
            if ((round(items[i].y) < psky * 32) && (round(items[i].y)>=((psky-scry)*32))&&
                    ((round(items[i].x) < pskx * 32)) && (round(items[i].x)>=((pskx-scrx)*32)))
            {

                pics.draw(11,
                        (round(items[i].x))-((pskx-scrx)*32) + pushx-posx,
                        (round(items[i].y))-((psky-scry)*32) + pushy-posy,
                        (items[i].value-1) * 4 + itmframe,
                        true,
                        1.0f,
                        1.0f,
                        0.0,
                        COLOR(r,g,b, 1.0f),
                        COLOR(r,g,b, 1.0f)
                        );
            }
        }
        else if ((round(items[i].y) < psky * TILE_WIDTH) && (round(items[i].y) >= ((psky-scry) * TILE_WIDTH))&&
                ((round(items[i].x) < pskx * TILE_WIDTH)) && (round(items[i].x) >= ((pskx-scrx) * TILE_WIDTH)))
        {

            pics.draw(7,
                    (round(items[i].x))-((pskx-scrx) * TILE_WIDTH) + pushx-posx,
                    (round(items[i].y))-((psky-scry) * TILE_WIDTH) + pushy-posy,
                    items[i].value - ITEM_AMMO_PACK,
                    true,
                    1.0f,1.0f,0.0,
                    COLOR(r,g,b, 1.f),
                    COLOR(r,g,b, 1.f)
                    );
        }

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
