#include "MapGenerator.h"
#include <cstdio>
#include <cstdlib>
#include "map.h"

MapGenerator::~MapGenerator()
{
    erase(&root);
    printf("not erased nodes: %d\n", newCount - deleteCount);
}


void MapGenerator::generate(CMap* map)
{
    divide(&root);
    makeRoom(&root, map);
    addTunels(&root, map);
}

void MapGenerator::divide(BSPTreeNode* parent)
{

    const int MIN_ROOM_SIZE = 16;

    if (parent->width < MIN_ROOM_SIZE && parent->height < MIN_ROOM_SIZE)
    {
        return;
    }


    DivisionType divType = ((rand() % 1000) % 2 == 1) ? DIV_VERTICAL : DIV_HORIZONTAL;

    if (divType == DIV_VERTICAL && parent->height < MIN_ROOM_SIZE)
    {
        if (parent->width < MIN_ROOM_SIZE)
        {
            return;
        }
        else
        {
            divType = DIV_HORIZONTAL;
        }
    }

    if (divType == DIV_HORIZONTAL && parent->width < MIN_ROOM_SIZE)
    {
        if (parent->height < MIN_ROOM_SIZE)
        {
            return;
        }
        else
        {
            divType = DIV_VERTICAL;
        }
    }


    if (divType == DIV_VERTICAL ) //vertical split
    {
        parent->left = new BSPTreeNode;
        parent->right = new BSPTreeNode;
        newCount += 2;

        parent->divType = divType;
        parent->left->startx = parent->startx;
        parent->left->starty = parent->starty;
        parent->left->height = 5 + (rand() % (parent->height / 4));
        parent->left->width = parent->width;

        parent->right->startx = parent->left->startx;
        parent->right->starty = parent->left->starty + parent->left->height;
        parent->right->width = parent->width;
        parent->right->height = parent->height - parent->left->height;
    }
    else
    {

        parent->left = new BSPTreeNode;
        parent->right = new BSPTreeNode;
        newCount += 2;

        parent->divType = divType;
        parent->left->startx = parent->startx;
        parent->left->starty = parent->starty;
        parent->left->height = parent->height;
        parent->left->width = parent->width / 3 + (rand() % (parent->width / 4));

        parent->right->startx = parent->startx + parent->left->width;
        parent->right->starty = parent->starty;
        parent->right->width = parent->width - parent->left->width;
        parent->right->height = parent->height;
    }

    divide(parent->left);
    divide(parent->right);

}

void MapGenerator::makeRoom(BSPTreeNode* parent, CMap* map)
{

    const int MIN_ROOM_WIDTH = 5;

    if (parent->left == nullptr && parent->right == nullptr) // this is the leaf node
    {

        int roomPosY = 1;

        int maxRandomHeightAddition = (parent->height - 4 < 0) ? 0 : parent->height - 4;
        int maxRandomWidthAddition = (parent->width - MIN_ROOM_WIDTH < 0) ? 0 : parent->width - MIN_ROOM_WIDTH;
        int roomHeight =  4  + rand() % maxRandomHeightAddition;
        int roomWidth = MIN_ROOM_WIDTH + rand() % maxRandomWidthAddition;

        for (int i = parent->starty + roomPosY; i < parent->starty + roomPosY + roomHeight; ++i)
        {
            for (int a = parent->startx; a < parent->startx + roomWidth; ++a)
            {
                if (i < (int)map->height() && a < (int)map->width())
                {

                    if (a == parent->startx && i > parent->starty)
                    {
                        map->tiles[i][a] = TILE_V_WALL;
                    }
                    else if (a == parent->startx + roomWidth - 1)
                    {
                        map->tiles[i][a] = TILE_V_WALL;
                    }
                    else
                    {
                        map->tiles[i][a] = 37;
                    }
                }
            }
        }

        int roomTopY = parent->starty;


        for (int a = parent->startx + 1; a < parent->startx + roomWidth - 1; ++a)
        {
            if (roomTopY < (int)map->height() && a < (int)map->width())
            {
                map->tiles[roomTopY][a] = TILE_H_WALL;
            }
        }

        int roomBottomY = parent->starty + roomHeight;

        for (int a = parent->startx + 1; a < parent->startx + roomWidth - 1; ++a)
        {
            if (roomBottomY < (int)map->height() && a < (int)map->width())
            {
                map->tiles[roomBottomY][a] = TILE_H_WALL;
            }
        }

        //corners
        map->tiles[parent->starty][parent->startx] = 22;
        map->tiles[parent->starty][parent->startx + roomWidth - 1] = 23;
        map->tiles[roomBottomY][parent->startx + roomWidth - 1] = 25;
        map->tiles[roomBottomY][parent->startx] = 24;

        return;
    }
    else
    {
        makeRoom(parent->left, map);
        makeRoom(parent->right, map);
    }

}

void MapGenerator::addTunels(BSPTreeNode* parent, CMap* map)
{
    switch(parent->divType)
    {
        case DIV_NONE:
            {
                return;
            }

        case DIV_VERTICAL:
            {
                for (int i = parent->starty + 1; i < parent->starty + parent->height - 1; ++i)
                {

                    if (map->tiles[i][parent->startx + 1] == 21 )
                    {

                        if (rand() % 10 == 1)
                        {
                            map->tiles[i][parent->startx + 1] = 33;
                            map->tiles[i][parent->startx + 2] = 37;
                            map->tiles[i][parent->startx + 3] = 32;
                        }
                        else
                        {
                            map->tiles[i][parent->startx + 2] = 69;
                        }
                    }
                    else
                    {
                        map->tiles[i][parent->startx + 2] = 37;
                    }
                }

            } break;

        case DIV_HORIZONTAL:
            {
                for (int i = parent->startx + 1; i < parent->startx + parent->width - 1; ++i)
                {
                    if (map->tiles[parent->starty + 1][i] == 20 )
                    {
                        //map->tiles[parent->starty + 1][i] = 34;
                        map->tiles[parent->starty + 2][i] = 71;
                        //map->tiles[parent->starty + 3][i] = 31;
                    }
                    else
                    {
                        map->tiles[parent->starty + 2][i] = 37;
                    }
                }

            } break;
    }

    addTunels(parent->left, map);
    addTunels(parent->right, map);
}


void MapGenerator::erase(BSPTreeNode* parent)
{
    if (parent->left)
    {
        erase(parent->left);
        delete parent->left;
        parent->left = nullptr;
        ++deleteCount;
    }

    if (parent->right)
    {
        erase(parent->right);
        delete parent->right;
        parent->right = nullptr;
        ++deleteCount;
    }

}
