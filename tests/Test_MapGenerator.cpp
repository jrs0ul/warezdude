#include <gtest/gtest.h>

#include "../src/MapGenerator.h"
#include "../src/Consts.h"
#include "../src/map.h"


void makeDummyMap(CMap* map, unsigned w = 10, unsigned h = 10)
{
    map->tiles = new unsigned char* [h];
    map->_height = h;
    map->_width = w;

    for (unsigned i = 0; i < map->_height; ++i)
    {
        map->tiles[i] = new unsigned char[w];

        for (unsigned a = 0; a < map->_width; ++a)
        {
            map->tiles[i][a] = 0;
        }
    }


}

void deleteDummyMap(CMap* map)
{
   for (unsigned a = 0; a < map->_height; ++a)
    {
        delete []map->tiles[a];
    }

    delete []map->tiles;
}

TEST(MapGenerator, makeWallsPretty_BasicRoom)
{

    CMap map;

    makeDummyMap(&map);

    for (int i = 1; i < 7; ++i)
    {
        map.tiles[1][i] = TILE_WALL;
        map.tiles[5][i] = TILE_WALL;
    }

    for (int i = 1; i < 6; ++i)
    {
        map.tiles[i][1] = TILE_WALL;
        map.tiles[i][6] = TILE_WALL;
    }

    map.tiles[5][3] = 0;


    //0000000000
    //0111111000
    //0100001000
    //0100001000
    //0100001000
    //0110111000
    //0000000000
    //0000000000
    //0000000000
    //0000000000

    MapGenerator gen(10, 10);
    gen.makeWallsPretty(&map);


    EXPECT_EQ(map.tiles[1][1], TILE_CORNER_TL);
    EXPECT_EQ(map.tiles[2][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[3][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[4][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[5][1], TILE_CORNER_BL);
    EXPECT_EQ(map.tiles[5][2], TILE_WALL_TAIL_RIGHT);
    EXPECT_EQ(map.tiles[5][3], 0);
    EXPECT_EQ(map.tiles[5][4], TILE_WALL_TAIL_LEFT);
    EXPECT_EQ(map.tiles[5][5], TILE_H_WALL);
    EXPECT_EQ(map.tiles[5][6], TILE_CORNER_BR);
    EXPECT_EQ(map.tiles[4][6], TILE_V_WALL);
    EXPECT_EQ(map.tiles[3][6], TILE_V_WALL);
    EXPECT_EQ(map.tiles[2][6], TILE_V_WALL);
    EXPECT_EQ(map.tiles[1][6], TILE_CORNER_TR);
    EXPECT_EQ(map.tiles[1][2], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][3], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][4], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][5], TILE_H_WALL);

    deleteDummyMap(&map);
}

TEST(MapGenerator, makeWallsPretty_AdjacentWallsVertical)
{

    CMap map;
    makeDummyMap(&map);

    //0000000000
    //0110000000
    //0110000000
    //0110000000
    //0110000000
    //0000011000
    //0000011000
    //0000000000
    //0000000000
    //0000000000

    map.tiles[1][1] = TILE_WALL;
    map.tiles[1][2] = TILE_WALL;
    map.tiles[2][1] = TILE_WALL;
    map.tiles[2][2] = TILE_WALL;
    map.tiles[3][1] = TILE_WALL;
    map.tiles[3][2] = TILE_WALL;
    map.tiles[4][1] = TILE_WALL;
    map.tiles[4][2] = TILE_WALL;

    map.tiles[5][5] = TILE_WALL;
    map.tiles[5][6] = TILE_WALL;
    map.tiles[6][5] = TILE_WALL;
    map.tiles[6][6] = TILE_WALL;

    MapGenerator gen(10, 10);
    gen.makeWallsPretty(&map);


    EXPECT_EQ(map.tiles[1][1], TILE_CORNER_TL);
    EXPECT_EQ(map.tiles[1][2], TILE_CORNER_TR);
    EXPECT_EQ(map.tiles[2][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[2][2], TILE_V_WALL);
    EXPECT_EQ(map.tiles[3][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[3][2], TILE_V_WALL);
    EXPECT_EQ(map.tiles[4][1], TILE_CORNER_BL);
    EXPECT_EQ(map.tiles[4][2], TILE_CORNER_BR);

    EXPECT_EQ(map.tiles[5][5], TILE_CORNER_TL);
    EXPECT_EQ(map.tiles[6][5], TILE_CORNER_BL);
    EXPECT_EQ(map.tiles[5][6], TILE_CORNER_TR);
    EXPECT_EQ(map.tiles[6][6], TILE_CORNER_BR);

    deleteDummyMap(&map);
}

TEST(MapGenerator, makeWallsPretty_AdjacentWallsHorizontal)
{

    CMap map;
    makeDummyMap(&map);

    //0000000000
    //0111110000
    //0111110000
    //0000000000
    //0000000000
    //0000000000
    //0000000000
    //0000000000
    //0000000000
    //0000000000

    map.tiles[1][1] = TILE_WALL;
    map.tiles[1][2] = TILE_WALL;
    map.tiles[1][3] = TILE_WALL;
    map.tiles[1][4] = TILE_WALL;
    map.tiles[1][5] = TILE_WALL;

    map.tiles[2][1] = TILE_WALL;
    map.tiles[2][2] = TILE_WALL;
    map.tiles[2][3] = TILE_WALL;
    map.tiles[2][4] = TILE_WALL;
    map.tiles[2][5] = TILE_WALL;

    MapGenerator gen(10, 10);
    gen.makeWallsPretty(&map);


    EXPECT_EQ(map.tiles[1][1], TILE_CORNER_TL);
    EXPECT_EQ(map.tiles[1][2], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][3], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][4], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][5], TILE_CORNER_TR);

    EXPECT_EQ(map.tiles[2][1], TILE_CORNER_BL);
    EXPECT_EQ(map.tiles[2][2], TILE_H_WALL);
    EXPECT_EQ(map.tiles[2][3], TILE_H_WALL);
    EXPECT_EQ(map.tiles[2][4], TILE_H_WALL);
    EXPECT_EQ(map.tiles[2][5], TILE_CORNER_BR);

    deleteDummyMap(&map);
}

TEST(MapGenerator, makeWallsPretty_IsolatedLines)
{

    CMap map;
    makeDummyMap(&map);

    //0000000000
    //0111110000
    //0000000000
    //0100100010
    //0100100010
    //0100100000
    //0000100000
    //0010000000
    //0000001100
    //1000000000

    map.tiles[1][1] = TILE_WALL;
    map.tiles[1][2] = TILE_WALL;
    map.tiles[1][3] = TILE_WALL;
    map.tiles[1][4] = TILE_WALL;
    map.tiles[1][5] = TILE_WALL;

    map.tiles[3][1] = TILE_WALL;
    map.tiles[4][1] = TILE_WALL;
    map.tiles[5][1] = TILE_WALL;

    map.tiles[3][4] = TILE_WALL;
    map.tiles[4][4] = TILE_WALL;
    map.tiles[5][4] = TILE_WALL;
    map.tiles[6][4] = TILE_WALL;

    map.tiles[3][8] = TILE_WALL;
    map.tiles[4][8] = TILE_WALL;

    map.tiles[7][3] = TILE_WALL;

    map.tiles[8][6] = TILE_WALL;
    map.tiles[8][7] = TILE_WALL;

    map.tiles[9][0] = TILE_WALL;


    MapGenerator gen(10, 10);
    gen.makeWallsPretty(&map);


    EXPECT_EQ(map.tiles[1][1], TILE_WALL_TAIL_LEFT);
    EXPECT_EQ(map.tiles[1][2], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][3], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][4], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][5], TILE_WALL_TAIL_RIGHT);


    EXPECT_EQ(map.tiles[3][1], TILE_WALL_TAIL_UP);
    EXPECT_EQ(map.tiles[4][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[5][1], TILE_WALL_TAIL_DOWN);

    EXPECT_EQ(map.tiles[3][4], TILE_WALL_TAIL_UP);
    EXPECT_EQ(map.tiles[4][4], TILE_V_WALL);
    EXPECT_EQ(map.tiles[5][4], TILE_V_WALL);
    EXPECT_EQ(map.tiles[6][4], TILE_WALL_TAIL_DOWN);


    EXPECT_EQ(map.tiles[3][8], TILE_WALL_TAIL_UP);
    EXPECT_EQ(map.tiles[4][8], TILE_WALL_TAIL_DOWN);

    EXPECT_EQ(map.tiles[7][3], TILE_WALL);

    EXPECT_EQ(map.tiles[8][6], TILE_WALL_TAIL_LEFT);
    EXPECT_EQ(map.tiles[8][7], TILE_WALL_TAIL_RIGHT);

    EXPECT_EQ(map.tiles[9][0], TILE_WALL);


    deleteDummyMap(&map);
}


TEST(MapGenerator, makeWallsPretty_WrinklyCorner1)
{
    CMap map;
    makeDummyMap(&map);

    //0000000000
    //0000000000
    //0000000111
    //0000000100
    //1111110100
    //0001110111
    //0001000001
    //0001111111
    //0000000000
    //0000000000

    map.tiles[2][7] = TILE_WALL;
    map.tiles[2][8] = TILE_WALL;
    map.tiles[2][9] = TILE_WALL;
    map.tiles[3][7] = TILE_WALL;

    for (int i = 0; i < 6; ++i)
    {
        map.tiles[4][i] = TILE_WALL;
    }

    map.tiles[4][7] = TILE_WALL;

    map.tiles[5][3] = TILE_WALL;
    map.tiles[5][4] = TILE_WALL;
    map.tiles[5][5] = TILE_WALL;

    map.tiles[5][7] = TILE_WALL;
    map.tiles[5][8] = TILE_WALL;
    map.tiles[5][9] = TILE_WALL;

    map.tiles[6][3] = TILE_WALL;
    map.tiles[6][9] = TILE_WALL;

    for (int i = 3; i < 10; ++i)
    {
        map.tiles[7][i] = TILE_WALL;
    }


    MapGenerator gen(10, 10);
    gen.makeWallsPretty(&map);

    EXPECT_EQ(map.tiles[2][7], TILE_CORNER_TL);
    //TODO: FINISH THIS

    deleteDummyMap(&map);
}

TEST(MapGenerator, makeWallsPretty_RoomsWithTShapes)
{

    CMap map;
    makeDummyMap(&map);

    //0000000000
    //0111110000
    //0100010000
    //0100011111
    //0100000000
    //0100011111
    //0100010000
    //1110111000
    //0000001000
    //0000001000

    for (int i = 1; i < 6; ++i)
    {
        map.tiles[1][i] = TILE_WALL;
    }

    for (int i = 1; i < 8; ++i)
    {
        map.tiles[i][1] = TILE_WALL;
    }

    for (int i = 5; i < 10; ++i)
    {
        map.tiles[3][i] = TILE_WALL;
        map.tiles[5][i] = TILE_WALL;
    }

    map.tiles[2][5] = TILE_WALL;

    map.tiles[6][5] = TILE_WALL;
    map.tiles[7][5] = TILE_WALL;
    map.tiles[7][4] = TILE_WALL;
    map.tiles[7][0] = TILE_WALL;
    map.tiles[7][1] = TILE_WALL;
    map.tiles[7][2] = TILE_WALL;

    map.tiles[7][6] = TILE_WALL;

    map.tiles[8][6] = TILE_WALL;
    map.tiles[9][6] = TILE_WALL;

    MapGenerator gen(10, 10);
    gen.makeWallsPretty(&map);

    EXPECT_EQ(map.tiles[1][1], TILE_CORNER_TL);
    EXPECT_EQ(map.tiles[1][2], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][3], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][4], TILE_H_WALL);
    EXPECT_EQ(map.tiles[1][5], TILE_CORNER_TR);
    EXPECT_EQ(map.tiles[2][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[2][5], TILE_V_WALL);
    EXPECT_EQ(map.tiles[3][9], TILE_WALL_TAIL_RIGHT);
    EXPECT_EQ(map.tiles[3][8], TILE_H_WALL);
    EXPECT_EQ(map.tiles[3][7], TILE_H_WALL);
    EXPECT_EQ(map.tiles[3][6], TILE_H_WALL);
    EXPECT_EQ(map.tiles[3][5], TILE_CORNER_BL);
    EXPECT_EQ(map.tiles[3][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[4][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[5][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[5][5], TILE_CORNER_TL);
    EXPECT_EQ(map.tiles[5][6], TILE_H_WALL);
    EXPECT_EQ(map.tiles[5][7], TILE_H_WALL);
    EXPECT_EQ(map.tiles[5][8], TILE_H_WALL);
    EXPECT_EQ(map.tiles[5][9], TILE_WALL_TAIL_RIGHT);

    EXPECT_EQ(map.tiles[6][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[6][5], TILE_V_WALL);

    EXPECT_EQ(map.tiles[7][0], TILE_WALL_TAIL_LEFT);
    EXPECT_EQ(map.tiles[7][1], TILE_TSHAPE_180);
    EXPECT_EQ(map.tiles[7][2], TILE_WALL_TAIL_RIGHT);
    EXPECT_EQ(map.tiles[7][4], TILE_WALL_TAIL_LEFT);
    EXPECT_EQ(map.tiles[7][5], TILE_TSHAPE_180);
    EXPECT_EQ(map.tiles[7][6], TILE_CORNER_TR);
    EXPECT_EQ(map.tiles[8][6], TILE_V_WALL);
    EXPECT_EQ(map.tiles[9][6], TILE_WALL_TAIL_DOWN);

    deleteDummyMap(&map);
}


TEST(MapGenerator, makeWallsPretty_FancyMap)
{

    CMap map;
    makeDummyMap(&map, 13, 10);


    //0000000000001
    //0000000000001
    //0000000000111
    //1110111111100
    //0110110111111
    //0100011100001
    //0100000000001
    //0110111111011
    //0010100001010
    //0010100001010

    map.tiles[0][12] = TILE_WALL;
    map.tiles[1][12] = TILE_WALL;
    map.tiles[2][10] = TILE_WALL;
    map.tiles[2][11] = TILE_WALL;
    map.tiles[2][12] = TILE_WALL;
    map.tiles[3][0] = TILE_WALL;
    map.tiles[3][1] = TILE_WALL;
    map.tiles[3][2] = TILE_WALL;

    for (int i = 4; i < 11; ++i)
    {
        map.tiles[3][i] = TILE_WALL;
    }

    map.tiles[4][1] = TILE_WALL;
    map.tiles[4][2] = TILE_WALL;
    map.tiles[4][4] = TILE_WALL;
    map.tiles[4][5] = TILE_WALL;

    for (int i = 7; i < 13; ++i)
    {
        map.tiles[4][i] = TILE_WALL;
    }

    map.tiles[5][1] = TILE_WALL;
    map.tiles[5][5] = TILE_WALL;
    map.tiles[5][6] = TILE_WALL;
    map.tiles[5][7] = TILE_WALL;
    map.tiles[5][12] = TILE_WALL;
    map.tiles[6][1] = TILE_WALL;
    map.tiles[6][12] = TILE_WALL;
    map.tiles[7][1] = TILE_WALL;
    map.tiles[7][2] = TILE_WALL;

    for (int i = 4; i < 10; ++i)
    {
        map.tiles[7][i] = TILE_WALL;
    }

    map.tiles[7][11] = TILE_WALL;
    map.tiles[7][12] = TILE_WALL;

    map.tiles[8][2] = TILE_WALL;
    map.tiles[8][4] = TILE_WALL;
    map.tiles[8][9] = TILE_WALL;
    map.tiles[8][11] = TILE_WALL;

    map.tiles[9][2] = TILE_WALL;
    map.tiles[9][4] = TILE_WALL;
    map.tiles[9][9] = TILE_WALL;
    map.tiles[9][11] = TILE_WALL;


    MapGenerator gen(13, 10);
    gen.makeWallsPretty(&map);

    EXPECT_EQ(map.tiles[0][12], TILE_WALL_TAIL_UP);
    EXPECT_EQ(map.tiles[1][12], TILE_V_WALL);

    EXPECT_EQ(map.tiles[2][10], TILE_CORNER_TL);
    EXPECT_EQ(map.tiles[2][11], TILE_H_WALL);
    EXPECT_EQ(map.tiles[2][12], TILE_CORNER_BR);

    EXPECT_EQ(map.tiles[3][0], TILE_WALL_TAIL_LEFT);
    EXPECT_EQ(map.tiles[3][1], TILE_H_WALL);
    EXPECT_EQ(map.tiles[3][2], TILE_CORNER_TR);

    EXPECT_EQ(map.tiles[3][4], TILE_CORNER_TL);

    EXPECT_EQ(map.tiles[3][5], TILE_H_WALL);
    EXPECT_EQ(map.tiles[3][6], TILE_H_WALL);
    EXPECT_EQ(map.tiles[3][7], TILE_H_WALL);
    EXPECT_EQ(map.tiles[3][8], TILE_H_WALL);
    EXPECT_EQ(map.tiles[3][9], TILE_H_WALL);
    EXPECT_EQ(map.tiles[3][10], TILE_CORNER_BR);
    EXPECT_EQ(map.tiles[4][1], TILE_CORNER_TL);
    EXPECT_EQ(map.tiles[4][2], TILE_CORNER_BR);
    EXPECT_EQ(map.tiles[4][4], TILE_CORNER_BL);
    EXPECT_EQ(map.tiles[4][5], TILE_CORNER_TR);
    EXPECT_EQ(map.tiles[4][7], TILE_CORNER_TL);
    EXPECT_EQ(map.tiles[4][7], TILE_CORNER_TL);

    EXPECT_EQ(map.tiles[4][8], TILE_H_WALL);
    EXPECT_EQ(map.tiles[4][9], TILE_H_WALL);
    EXPECT_EQ(map.tiles[4][10], TILE_H_WALL);
    EXPECT_EQ(map.tiles[4][11], TILE_H_WALL);
    EXPECT_EQ(map.tiles[4][12], TILE_CORNER_TR);

    EXPECT_EQ(map.tiles[5][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[5][5], TILE_CORNER_BL);
    EXPECT_EQ(map.tiles[5][6], TILE_H_WALL);
    EXPECT_EQ(map.tiles[5][7], TILE_CORNER_BR);
    EXPECT_EQ(map.tiles[5][12], TILE_V_WALL);

    EXPECT_EQ(map.tiles[6][1], TILE_V_WALL);
    EXPECT_EQ(map.tiles[6][12], TILE_V_WALL);

    EXPECT_EQ(map.tiles[7][1], TILE_CORNER_BL);
    EXPECT_EQ(map.tiles[7][2], TILE_CORNER_TR);
    EXPECT_EQ(map.tiles[7][4], TILE_CORNER_TL);

    EXPECT_EQ(map.tiles[7][5], TILE_H_WALL);
    EXPECT_EQ(map.tiles[7][6], TILE_H_WALL);
    EXPECT_EQ(map.tiles[7][7], TILE_H_WALL);
    EXPECT_EQ(map.tiles[7][8], TILE_H_WALL);
    EXPECT_EQ(map.tiles[7][9], TILE_CORNER_TR);
    EXPECT_EQ(map.tiles[7][11], TILE_CORNER_TL);
    EXPECT_EQ(map.tiles[7][12], TILE_CORNER_BR);
    
    EXPECT_EQ(map.tiles[8][2], TILE_V_WALL);
    EXPECT_EQ(map.tiles[8][4], TILE_V_WALL);
    EXPECT_EQ(map.tiles[8][9], TILE_V_WALL);
    EXPECT_EQ(map.tiles[8][11], TILE_V_WALL);
    
    EXPECT_EQ(map.tiles[9][2], TILE_WALL_TAIL_DOWN);
    EXPECT_EQ(map.tiles[9][4], TILE_WALL_TAIL_DOWN);
    EXPECT_EQ(map.tiles[9][9], TILE_WALL_TAIL_DOWN);
    EXPECT_EQ(map.tiles[9][11], TILE_WALL_TAIL_DOWN);

    deleteDummyMap(&map);
}

TEST(MapGenerator, makeWallsPretty_WrinklyCorner2)
{
    CMap map;
    makeDummyMap(&map);

    //0000001000
    //0001111000
    //0001000000
    //1101000000
    //0101110111
    //0111110001
    //0000000001
    //0111111111
    //0100000000
    //0100000000

    map.tiles[0][6] = TILE_WALL;

    for (int i = 3; i < 7; ++i)
    {
        map.tiles[1][i] = TILE_WALL;
    }

    map.tiles[2][3] = TILE_WALL;

    map.tiles[3][0] = TILE_WALL;
    map.tiles[3][1] = TILE_WALL;
    map.tiles[3][3] = TILE_WALL;

    //TODO: FINISH FILLING

    MapGenerator gen(10, 10);
    gen.makeWallsPretty(&map);

    EXPECT_EQ(map.tiles[0][6], TILE_WALL_TAIL_UP);
    //TODO: FINISH CHECKS

    deleteDummyMap(&map);
}



