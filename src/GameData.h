#pragma once

#include "DArray.h"


struct GameDescription
{
    char name[128];
    char description[256];
    char description1[256];
    char effect[256];
    char effect1[256];
};

class GameData
{

    DArray<GameDescription> games;

public:
    bool load(const char* path);
    GameDescription* getGame(unsigned index);
    ~GameData(){games.destroy();}
};
