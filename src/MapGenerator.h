#pragma once

#include "DArray.h"

class Vector3D;

enum DivisionType
{
    DIV_NONE,
    DIV_VERTICAL,
    DIV_HORIZONTAL
};

enum CorridorDirections
{
    DIR_NONE = 0,
    DIR_RIGHT,
    DIR_LEFT,
    DIR_DOWN,
    DIR_UP
};

struct BSPTreeNode
{
    DArray<Vector3D> connectionPoints;

    int startx;
    int starty;
    int width;
    int height;

    int roomPosX;
    int roomPosY;
    int roomWidth;
    int roomHeight;

    DivisionType divType;

    BSPTreeNode* left;
    BSPTreeNode* right;

    bool connectedWithTunel;

    BSPTreeNode()
    : roomPosX(-1)
    , roomPosY(-1)
    , roomWidth(-1)
    , roomHeight(-1)
    , divType(DIV_NONE)
    , left(nullptr)
    , right(nullptr)
    , connectedWithTunel(false)
    {
    }
};

class CMap;

class MapGenerator
{
public:
    MapGenerator(int width, int height)
    {
        root.startx = 0;
        root.starty = 0;
        root.width = width;
        root.height = height;
        newCount = 0;
        deleteCount = 0;
    }

    ~MapGenerator();

    void generate(CMap* map);

private:
    void divide(BSPTreeNode* parent);
    void makeRoom(BSPTreeNode* node, CMap* map);
    void connectRooms(BSPTreeNode* node, CMap* map);

    int getDepth(BSPTreeNode* node);

    void erase(BSPTreeNode* parent);

private:
    BSPTreeNode root;
    int newCount;
    int deleteCount;
};
