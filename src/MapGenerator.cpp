#include "MapGenerator.h"
#include <cstdio>
#include <cstdlib>
#include "Vectors.h"
#include "map.h"

MapGenerator::~MapGenerator()
{
    roomList.destroy();
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

    makeWallsPretty(map);
    putDoorsToOutside(map);
}

BSPTreeNode* MapGenerator::getRoomNode(unsigned idx)
{
    return roomList[idx];
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

        roomList.add(node);

        node->connectionPoints.destroy();

        int xRoomMiddle = node->roomWidth / 2;
        int yRoomMiddle = node->roomHeight / 2;

        Vector3D v(node->startx + node->roomPosX + xRoomMiddle, node->starty + node->roomPosY + yRoomMiddle, 0);
        node->connectionPoints.add(v);


        for (int i = node->starty + node->roomPosY; i < node->starty + node->roomPosY + node->roomHeight; ++i)
        {
            for (int a = node->startx + node->roomPosX; a < node->startx + node->roomPosX + node->roomWidth; ++a)
            {
                if (i < (int)map->height() && a < (int)map->width())
                {

                    if (a == node->startx + node->roomPosX)
                    {
                        map->tiles[i][a] = TILE_WALL;
                    }
                    else if (a == node->startx + node->roomPosX + node->roomWidth - 1)
                    {
                        map->tiles[i][a] = TILE_WALL;
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


        //horizontal walls
        for (int a = node->startx + node->roomPosX; a < node->startx + node->roomPosX + node->roomWidth; ++a)
        {
            if (roomTopY < (int)map->height() && a < (int)map->width() && roomBottomY < (int)map->height())
            {
                map->tiles[roomTopY][a] = TILE_WALL;
                map->tiles[roomBottomY][a] = TILE_WALL;
            }
        }

      
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
                    //top wall
                    PutWall(map, lx, ly - 1, TILE_WALL);

                    //bottom wall
                    PutWall(map, lx, ly + 1, TILE_WALL);

                } break;
                case DIR_UP:
                {
                    //left wall
                    PutWall(map, lx - 1, ly, TILE_WALL);

                    //right wall
                    PutWall(map, lx + 1, ly, TILE_WALL);

                    if (oldDir == DIR_RIGHT || oldDir == DIR_LEFT)
                    {
                        PutWall(map, lx - 1, oldLy, TILE_WALL);
                        PutWall(map, lx + 1, oldLy, TILE_WALL);
                        PutWall(map, lx - 1, oldLy + 1, TILE_WALL);
                        PutWall(map, lx + 1, oldLy + 1, TILE_WALL);
                    }

                } break;
                case DIR_DOWN :
                {

                    //Left vertical wall
                    PutWall(map, lx - 1, ly, TILE_WALL); //  regular vertical wall

                    //Right vertical wall
                    PutWall(map, lx + 1, ly, TILE_WALL);  //  regular vertical wall

                    if (oldDir == DIR_RIGHT || oldDir == DIR_LEFT)
                    {
                        PutWall(map, lx - 1, oldLy, TILE_WALL);
                        PutWall(map, lx + 1, oldLy, TILE_WALL);
                        PutWall(map, lx - 1, oldLy - 1, TILE_WALL);
                        PutWall(map, lx + 1, oldLy - 1, TILE_WALL);
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

void MapGenerator::makeWallsPretty(CMap* map)
{
    for (unsigned i = 0; i < map->height(); ++i)
    {

        for (unsigned a = 0; a < map->width(); ++a)
        {
            if (map->tiles[i][a] == TILE_WALL)
            {
                // since we go from top to bottom, from left to right,
                // right and bottom tiles in most cases will be TILE_WALL if there is a wall there!

                const unsigned char leftTile = (a - 1 < map->width())    ? map->tiles[i][a - 1]  : 0;
                const unsigned char rightTile = (a + 1 < map->width())   ? map->tiles[i][a + 1]  : 0;
                const unsigned char topTile = (i - 1 < map->height())    ? map->tiles[i - 1][a]  : 0;
                const unsigned char bottomTile = (i + 1 < map->height()) ? map->tiles[i + 1][a]  : 0;

                const unsigned char topRightTile = (i - 1 < map->height() && a + 1 < map->width()) ? map->tiles[i - 1][a + 1] : 0;
                const unsigned char bottomLeftTile = (i + 1 < map->height() && a - 1 < map->width()) ? map->tiles[i + 1][a - 1] : 0;
                const unsigned char bottomRightTile = (i + 1 < map->height() && a + 1 < map->width()) ? map->tiles[i + 1][a + 1] : 0;

                if (rightTile == TILE_WALL && 
                        (leftTile == TILE_WALL || leftTile == TILE_H_WALL ||
                         leftTile == TILE_CORNER_TL || leftTile == TILE_CORNER_BL ||
                         leftTile == TILE_WALL_TAIL_LEFT || leftTile == TILE_TSHAPE_L90 ||
                         leftTile == TILE_TSHAPE || leftTile == TILE_TSHAPE_180) &&
                        (topTile != TILE_V_WALL && topTile != TILE_CORNER_TL) && 
                        !(bottomTile == TILE_WALL && bottomLeftTile != TILE_WALL && bottomRightTile != TILE_WALL))
                {
                    map->tiles[i][a] = TILE_H_WALL;
                    continue;
                }

                if (!(rightTile == TILE_WALL && bottomRightTile != TILE_WALL) &&
                        (leftTile != TILE_WALL && leftTile != TILE_H_WALL && 
                         leftTile != TILE_CORNER_BL && leftTile != TILE_CORNER_TL) &&
                        (topTile == TILE_WALL || topTile == TILE_V_WALL ||
                         topTile == TILE_CORNER_TL || topTile == TILE_CORNER_TR ||
                         topTile == TILE_WALL_TAIL_UP || topTile == TILE_TSHAPE_L90 || 
                         topTile == TILE_TSHAPE_R90 || topTile == TILE_TSHAPE) &&
                        bottomTile == TILE_WALL)
                {
                    map->tiles[i][a] = TILE_V_WALL;
                    continue;
                }

                if ((leftTile != TILE_WALL && leftTile != TILE_H_WALL && leftTile != TILE_WALL_TAIL_LEFT) &&
                        (rightTile == TILE_WALL) &&
                        (topTile != TILE_V_WALL && topTile != TILE_CORNER_TL && 
                         topTile != TILE_CORNER_TR && topTile != TILE_WALL_TAIL_UP) &&
                        bottomTile == TILE_WALL)
                {
                    map->tiles[i][a] = TILE_CORNER_TL;
                    continue;
                }

                if ((leftTile == TILE_H_WALL || leftTile == TILE_WALL || leftTile == TILE_CORNER_BL || 
                      leftTile == TILE_CORNER_TL || leftTile == TILE_TSHAPE_180) &&
                        rightTile != TILE_WALL && 
                        bottomTile == TILE_WALL && 
                        (/*topTile != TILE_H_WALL &&*/ topTile != TILE_WALL_TAIL_UP && 
                         topTile != TILE_V_WALL && topTile != TILE_CORNER_TL))
                {
                    map->tiles[i][a] = TILE_CORNER_TR;
                    continue;
                }

                if ((topTile == TILE_V_WALL || topTile == TILE_CORNER_TR || topTile == TILE_CORNER_TL) && 
                        (leftTile != TILE_H_WALL && leftTile != TILE_WALL && leftTile != TILE_WALL_TAIL_LEFT) &&
                        bottomTile != TILE_WALL && rightTile == TILE_WALL)
                {
                    map->tiles[i][a] = TILE_CORNER_BL;
                    continue;
                }

                if (!(rightTile == TILE_WALL && topRightTile != TILE_V_WALL) && 
                        (leftTile == TILE_WALL || leftTile == TILE_H_WALL || leftTile == TILE_CORNER_BL || leftTile == TILE_CORNER_TL) &&
                        (topTile == TILE_V_WALL || topTile == TILE_CORNER_TL || topTile == TILE_CORNER_TR) /*&&
                        bottomTile != TILE_WALL*/)
                {
                    map->tiles[i][a] = TILE_CORNER_BR;
                    continue;
                }

                if ((topTile != TILE_WALL && topTile != TILE_V_WALL &&
                            topTile != TILE_CORNER_TR && topTile != TILE_CORNER_TL && topTile != TILE_WALL_TAIL_UP) &&
                        rightTile != TILE_WALL && bottomTile == TILE_WALL)
                {
                    map->tiles[i][a] = TILE_WALL_TAIL_UP;
                    continue;
                }

                if ((topTile == TILE_V_WALL || topTile == TILE_WALL || topTile == TILE_WALL_TAIL_UP || 
                            topTile == TILE_TSHAPE_L90 || topTile == TILE_TSHAPE_R90) &&
                        bottomTile != TILE_WALL &&
                        rightTile != TILE_WALL)
                {
                    map->tiles[i][a] = TILE_WALL_TAIL_DOWN;
                    continue;
                }

                if (rightTile == TILE_WALL && bottomTile != TILE_WALL && topTile != TILE_V_WALL)
                {
                    map->tiles[i][a] = TILE_WALL_TAIL_LEFT;
                    continue;
                }

                if (rightTile != TILE_WALL && bottomTile != TILE_WALL && 
                        (leftTile == TILE_H_WALL || leftTile == TILE_WALL_TAIL_LEFT || leftTile == TILE_CORNER_BL ||
                         leftTile == TILE_TSHAPE_180))
                {
                    map->tiles[i][a] = TILE_WALL_TAIL_RIGHT;
                    continue;
                }

                if (rightTile == TILE_WALL && 
                        (topTile == TILE_V_WALL || topTile == TILE_WALL_TAIL_UP) && bottomTile == TILE_WALL)
                {
                    map->tiles[i][a] = TILE_TSHAPE_L90;   // |--
                    continue;
                }

                if ((leftTile == TILE_H_WALL || leftTile == TILE_WALL_TAIL_LEFT) &&
                        rightTile != TILE_WALL && 
                        (topTile == TILE_V_WALL || topTile == TILE_WALL_TAIL_UP) &&
                        bottomTile == TILE_WALL)
                {
                    map->tiles[i][a] = TILE_TSHAPE_R90;   // --|
                    continue;
                }

                if ((leftTile == TILE_H_WALL || leftTile == TILE_WALL_TAIL_LEFT) &&
                        rightTile == TILE_WALL && bottomTile == TILE_WALL &&
                        topTile != TILE_V_WALL)
                {
                    map->tiles[i][a] = TILE_TSHAPE;   //  T
                    continue;
                }

                if (topTile == TILE_V_WALL && 
                        (leftTile == TILE_H_WALL || leftTile == TILE_WALL_TAIL_LEFT) &&
                        rightTile == TILE_WALL && 
                        bottomTile != TILE_WALL)
                {
                    map->tiles[i][a] = TILE_TSHAPE_180;  //  _|_
                    continue;
                }

                if (topTile == TILE_V_WALL && leftTile == TILE_H_WALL && rightTile == TILE_WALL && 
                        bottomTile == TILE_WALL)
                {
                    map->tiles[i][a] = TILE_WALL_CROSS;
                }


            }
        }

    }
}

void MapGenerator::putDoorsToOutside(CMap* map)
{
    for (unsigned i = 0; i < roomList.count(); ++i)
    {
        BSPTreeNode* room = roomList[i];

        unsigned leftDoorX = room->startx + room->roomPosX;
        unsigned leftDoorY = room->starty + room->roomPosY + room->roomHeight / 2;

        if (leftDoorX < map->width() && leftDoorY < map->height())
        {
            if (leftDoorX - 1 < map->width() && map->tiles[leftDoorY][leftDoorX] == TILE_V_WALL)
            {
                map->tiles[leftDoorY][leftDoorX] = TILE_V_DOOR_CONCRETE;
            }
        }

        unsigned rightDoorX = room->startx + room->roomPosX + room->roomWidth - 1;
        unsigned rightDoorY = leftDoorY;

        if (rightDoorX < map->width() && rightDoorY < map->height())
        {
            printf("%d\n", map->tiles[rightDoorY][rightDoorX]);
            if (rightDoorX + 1 < map->width() && map->tiles[rightDoorY][rightDoorX] == TILE_V_WALL)
            {
                map->tiles[rightDoorY][rightDoorX] = TILE_V_DOOR_CONCRETE;
            }
        }


    }
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
