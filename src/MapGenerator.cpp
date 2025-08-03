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
    erodeRoom(&root, map);
    addTunels(&root, map);
}

void MapGenerator::divide(BSPTreeNode* parent)
{
    if (parent->width < 8 || parent->height < 8)
    {
        return;
    }

    parent->left = new BSPTreeNode;
    parent->right = new BSPTreeNode;

    newCount += 2;


    if ((rand() % 1000) % 2 == 1) //vertical split
    {
        parent->divType = DIV_VERTICAL;
        parent->left->startx = parent->startx;
        parent->left->starty = parent->starty;
        parent->left->height = parent->height / 3 + (rand() % (parent->height / 4));
        parent->left->width = parent->width;

        parent->right->startx = parent->left->startx;
        parent->right->starty = parent->left->starty + parent->left->height;
        parent->right->width = parent->width;
        parent->right->height = parent->height - parent->left->height;
    }
    else
    {
        parent->divType = DIV_HORIZONTAL;
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

void MapGenerator::erodeRoom(BSPTreeNode* parent, CMap* map)
{
    if (parent->left == nullptr && parent->right == nullptr)
    {

        int roomPosY = 1;
        int roomHeight =  parent->height - 2;

        for (int i = parent->starty + roomPosY; i < parent->starty + roomPosY + roomHeight; ++i)
        {
            for (int a = parent->startx + 1; a < parent->startx + parent->width - 1; ++a)
            {
                if (i < (int)map->height() && a < (int)map->width())
                {
                    map->tiles[i][a] = TILE_DIRT;
                }
            }
        }


        return;
    }
    else
    {
        erodeRoom(parent->left, map);
        erodeRoom(parent->right, map);
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
                    map->tiles[i][parent->startx + 2] = 37;
                }
            } break;

        case DIV_HORIZONTAL:
            {
                for (int i = parent->startx + 1; i < parent->startx + parent->width - 1; ++i)
                {
                    map->tiles[parent->starty + 2][i] = 37;
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
