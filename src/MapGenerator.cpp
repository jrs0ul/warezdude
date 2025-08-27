#include "MapGenerator.h"
#include <cstdio>
#include <cstdlib>
#include "Vectors.h"
#include "map.h"

MapGenerator::~MapGenerator()
{
    erase(&root);
    printf("not erased nodes: %d\n", newCount - deleteCount);
}


void MapGenerator::generate(CMap* map)
{
    divide(&root);
    int depth = getDepth(&root);
    printf("tree depth: %d\n", depth);
    makeRoom(&root, map);
    for (int i = 0; i < depth; ++i)
    {
        connectRooms(&root, map);
    }
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

        node->connectedWithTunel = true;

        const int maxRandomHeightAddition = (node->height - MIN_ROOM_HEIGHT < 0) ? 0 : node->height - MIN_ROOM_HEIGHT;
        const int maxRandomWidthAddition = (node->width - MIN_ROOM_WIDTH < 0) ? 0 : node->width - MIN_ROOM_WIDTH;

        node->roomHeight = (maxRandomHeightAddition) ? MIN_ROOM_HEIGHT  + rand() % maxRandomHeightAddition : MIN_ROOM_HEIGHT;
        node->roomWidth = (maxRandomWidthAddition) ? MIN_ROOM_WIDTH + rand() % maxRandomWidthAddition : MIN_ROOM_WIDTH;
        node->roomPosY = (node->height - node->roomHeight) ? rand() % (node->height - node->roomHeight) : 0;
        node->roomPosX = (node->width - node->roomWidth) ? rand() % (node->width - node->roomWidth) : 0;

        node->connectionPoints.destroy();

        int xRoomMiddle = node->roomWidth / 2;
        int yRoomMiddle = node->roomHeight / 2;

        Vector3D v(node->startx + node->roomPosX + xRoomMiddle, node->starty + node->roomPosY + yRoomMiddle, 0);
        node->connectionPoints.add(v);


        int middle = node->starty + node->roomPosY + (node->roomHeight / 2);

        for (int i = node->starty + node->roomPosY; i < node->starty + node->roomPosY + node->roomHeight; ++i)
        {
            for (int a = node->startx + node->roomPosX; a < node->startx + node->roomPosX + node->roomWidth; ++a)
            {
                if (i < (int)map->height() && a < (int)map->width())
                {

                    if (a == node->startx + node->roomPosX)
                    {
                        if (i == middle && a > 1)
                        {
                            map->tiles[i][a] = TILE_V_DOOR_CONCRETE;
                        }
                        else
                        {
                            map->tiles[i][a] = TILE_V_WALL;
                        }
                    }
                    else if (a == node->startx + node->roomPosX + node->roomWidth - 1)
                    {
                        if (i == middle && a < (int)map->width() - 1)
                        {
                            map->tiles[i][a] = TILE_V_DOOR_CONCRETE;
                        }
                        else
                        {
                            map->tiles[i][a] = TILE_V_WALL;
                        }
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

void PutWall(CMap* map, int x, int y, unsigned char wallTile)
{
    if (y >= 0 && y < (int)map->height() &&
        x >= 0 && x < (int)map->width() && map->tiles[y][x] != TILE_CONCRETE_FLOOR)
    {
        map->tiles[y][x] = wallTile;
    }
}

bool IsTileSet(CMap* map, int x, int y, unsigned char tile)
{
    if (y >= 0 && y < (int)map->height() &&
        x >= 0 && x < (int)map->width() && map->tiles[y][x] == tile)
    {
        return true;
    }

    return false;

}

void MapGenerator::connectRooms(BSPTreeNode* node, CMap* map)
{
    BSPTreeNode* l = node->left;
    BSPTreeNode* r = node->right;

    if (node->divType == DIV_NONE)
    {
        return;
    }

    if (l->connectedWithTunel && r->connectedWithTunel && !node->connectedWithTunel)
    {

        int lPoint = rand() % l->connectionPoints.count();
        int rPoint = rand() % r->connectionPoints.count();

        int lx = l->connectionPoints[lPoint].x;
        int ly = l->connectionPoints[lPoint].y;

        int rx = r->connectionPoints[rPoint].x;
        int ry = r->connectionPoints[rPoint].y;

        CorridorDirections dir = DIR_NONE;
        CorridorDirections oldDir = dir;

        int oldLy = ly;
        bool topLeftWallCornerUsed_UP   = false;
        bool topLeftWallCornerUsed_DOWN = false;
        bool topRightWallCornerUsed_DOWN = false;

        while (lx != rx || ly != ry)
        {
            if (rx > lx)
            {
                ++lx;
                dir = DIR_RIGHT;
            }
            else if (rx < lx)
            {
                --lx;
                dir = DIR_LEFT;
            }
            else if (ry > ly)
            {
                ++ly;
                dir = DIR_DOWN;
            }
            else if (ry < ly)
            {
                --ly;
                dir = DIR_UP;
            }


            switch(dir)
            {
                case DIR_NONE : break;
                case DIR_LEFT :
                case DIR_RIGHT :
                {
                    PutWall(map, lx, ly - 1, TILE_H_WALL);
                    PutWall(map, lx, ly + 1, TILE_H_WALL);
                } break;
                case DIR_UP:
                {
                    if (IsTileSet(map, lx - 2, ly, TILE_H_WALL))
                    {
                        if (!topLeftWallCornerUsed_UP)
                        {
                            PutWall(map, lx - 1, ly, TILE_CORNER_BR);
                            topLeftWallCornerUsed_UP = true;
                        }
                        else
                        {
                            PutWall(map, lx - 1, ly, TILE_CORNER_TR);
                        }
                    }
                    else
                    {
                        PutWall(map, lx - 1, ly, TILE_V_WALL);
                    }


                    PutWall(map, lx + 1, ly, TILE_V_WALL);

                    if (oldDir == DIR_RIGHT || oldDir == DIR_LEFT)
                    {
                        puts("FROM HORIZ TO UP");
                        PutWall(map, lx - 1, oldLy, TILE_V_WALL);
                        PutWall(map, lx + 1, oldLy, TILE_V_WALL);
                        PutWall(map, lx - 1, oldLy + 1, TILE_CORNER_BR);
                        PutWall(map, lx + 1, oldLy + 1, TILE_CORNER_BR);
                    }

                } break;
                case DIR_DOWN :
                {
                    if (IsTileSet(map, lx - 2, ly, TILE_H_WALL)) //maybe we need to put a corner tile
                    {
                        if (!topLeftWallCornerUsed_DOWN)
                        {
                            PutWall(map, lx - 1, ly, TILE_CORNER_TR);
                            topLeftWallCornerUsed_DOWN = true;
                        }
                        else
                        {
                            PutWall(map, lx - 1, ly, TILE_CORNER_BR);
                        }
                    }
                    else
                    {
                        PutWall(map, lx - 1, ly, TILE_V_WALL); //  regular vertical wall
                    }

                    if (IsTileSet(map, lx + 2, ly, TILE_H_WALL))
                    {
                        if (!topRightWallCornerUsed_DOWN)
                        {
                            PutWall(map, lx + 1, ly, TILE_CORNER_TL);
                            topRightWallCornerUsed_DOWN = true;
                        }
                        else
                        {
                            PutWall(map, lx + 1, ly, TILE_CORNER_BL);
                        }
                    }
                    else
                    {
                        PutWall(map, lx + 1, ly, TILE_V_WALL);  //  regular vertical wall
                    }

                    if (oldDir == DIR_RIGHT || oldDir == DIR_LEFT)
                    {
                        puts("FROM HORIZ TO DOWN");
                        PutWall(map, lx - 1, oldLy, TILE_V_WALL);
                        PutWall(map, lx + 1, oldLy, TILE_V_WALL);
                        PutWall(map, lx - 1, oldLy - 1, TILE_V_WALL);
                        PutWall(map, lx + 1, oldLy - 1, TILE_V_WALL);
                    }
                }

            }

            if (ly >= 0 && ly < (int)map->height() && lx >= 0 && lx < (int)map->width())
            {
                map->tiles[ly][lx] = TILE_CONCRETE_FLOOR;
            }

            oldDir = dir;
            oldLy = ly;

        }

        node->connectedWithTunel = true;

        for (unsigned i = 0; i < l->connectionPoints.count(); ++i)
        {
            node->connectionPoints.add(l->connectionPoints[i]);
        }

        for (unsigned i = 0; i < r->connectionPoints.count(); ++i)
        {
            node->connectionPoints.add(r->connectionPoints[i]);
        }


    }


    connectRooms(node->left, map);
    connectRooms(node->right, map);
}


int MapGenerator::getDepth(BSPTreeNode* node)
{

    if (node == nullptr)
    {
        return -1;
    }

    int leftDepth = getDepth(node->left);
    int rightDepth = getDepth(node->right);

    int result = (leftDepth > rightDepth) ? leftDepth : rightDepth;

    return result + 1;
}


void MapGenerator::erase(BSPTreeNode* parent)
{

    parent->connectionPoints.destroy();

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
