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
    connectRooms(&root, map);
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

void MapGenerator::makeRoom(BSPTreeNode* node, CMap* map)
{

    const int MIN_ROOM_WIDTH = 5;
    const int MIN_ROOM_HEIGHT = 4;

    if (node->left == nullptr && node->right == nullptr) // this is the leaf node
    {
        const int maxRandomHeightAddition = (node->height - MIN_ROOM_HEIGHT < 0) ? 0 : node->height - MIN_ROOM_HEIGHT;
        const int maxRandomWidthAddition = (node->width - MIN_ROOM_WIDTH < 0) ? 0 : node->width - MIN_ROOM_WIDTH;

        node->roomHeight = (maxRandomHeightAddition) ? MIN_ROOM_HEIGHT  + rand() % maxRandomHeightAddition : MIN_ROOM_HEIGHT;
        node->roomWidth = (maxRandomWidthAddition) ? MIN_ROOM_WIDTH + rand() % maxRandomWidthAddition : MIN_ROOM_WIDTH;
        node->roomPosY = (node->height - node->roomHeight) ? rand() % (node->height - node->roomHeight) : 0;
        node->roomPosX = (node->width - node->roomWidth) ? rand() % (node->width - node->roomWidth) : 0;

        for (int i = node->starty + node->roomPosY; i < node->starty + node->roomPosY + node->roomHeight; ++i)
        {
            for (int a = node->startx + node->roomPosX; a < node->startx + node->roomPosX + node->roomWidth; ++a)
            {
                if (i < (int)map->height() && a < (int)map->width())
                {

                    if (a == node->startx + node->roomPosX)
                    {
                        map->tiles[i][a] = TILE_V_WALL;
                    }
                    else if (a == node->startx + node->roomPosX + node->roomWidth - 1)
                    {
                        map->tiles[i][a] = TILE_V_WALL;
                    }
                    else
                    {
                        map->tiles[i][a] = TILE_CONCRETE_FLOOR;
                    }
                }
            }
        }

        const int roomTopY    = node->starty + node->roomPosY;
        const int roomBottomY = node->starty + node->roomPosY + node->roomHeight - 1;
        const int roomRightX  = node->startx + node->roomPosX + node->roomWidth - 1;
        const int roomLeftX   = node->startx + node->roomPosX;


        //horizontal walls
        for (int a = node->startx + node->roomPosX; a < node->startx + node->roomPosX + node->roomWidth; ++a)
        {
            if (roomTopY < (int)map->height() && a < (int)map->width() && roomBottomY < (int)map->height())
            {
                map->tiles[roomTopY][a] = TILE_H_WALL;
                map->tiles[roomBottomY][a] = TILE_H_WALL;
            }
        }

        //corners
        map->tiles[roomTopY][roomLeftX]     = TILE_CORNER_TL;
        map->tiles[roomTopY][roomRightX]    = TILE_CORNER_TR;
        map->tiles[roomBottomY][roomRightX] = TILE_CORNER_BR;
        map->tiles[roomBottomY][roomLeftX]  = TILE_CORNER_BL;

        return;
    }
    else
    {
        makeRoom(node->left, map);
        makeRoom(node->right, map);
    }

}

void MapGenerator::connectRooms(BSPTreeNode* parent, CMap* map)
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

                    if (map->tiles[i][parent->startx + 1] == TILE_H_WALL )
                    {

                        if (rand() % 10 == 1)
                        {
                            map->tiles[i][parent->startx + 1] = 33;
                            map->tiles[i][parent->startx + 2] = TILE_CONCRETE_FLOOR;
                            map->tiles[i][parent->startx + 3] = 32;
                        }
                        else
                        {
                            map->tiles[i][parent->startx + 2] = 69;
                        }
                    }
                    else
                    {
                        map->tiles[i][parent->startx + 2] = TILE_CONCRETE_FLOOR;
                    }
                }

            } break;

        case DIV_HORIZONTAL:
            {
                for (int i = parent->startx + 1; i < parent->startx + parent->width - 1; ++i)
                {
                    if (map->tiles[parent->starty + 1][i] == TILE_V_WALL )
                    {
                        map->tiles[parent->starty + 2][i] = 71;
                    }
                    else
                    {
                        map->tiles[parent->starty + 2][i] = TILE_CONCRETE_FLOOR;
                    }
                }

            } break;
    }

    connectRooms(parent->left, map);
    connectRooms(parent->right, map);
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
