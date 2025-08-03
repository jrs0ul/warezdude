#pragma once


enum DivisionType
{
    DIV_NONE,
    DIV_VERTICAL,
    DIV_HORIZONTAL
};


struct BSPTreeNode
{
    int startx;
    int starty;
    int width;
    int height;
    DivisionType divType;

    BSPTreeNode* left;
    BSPTreeNode* right;

    BSPTreeNode()
    : divType(DIV_NONE)
    , left(nullptr)
    , right(nullptr)
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
    void erodeRoom(BSPTreeNode* parent, CMap* map);
    void addTunels(BSPTreeNode* parent, CMap* map);
    void erase(BSPTreeNode* parent);

private:
    BSPTreeNode root;
    int newCount;
    int deleteCount;
};
