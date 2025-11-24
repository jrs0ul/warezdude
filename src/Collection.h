#pragma once

#include <disarray/gui/BasicUiControl.h>
#include <vector>

class SpriteBatcher;

class Collection: public BasicControl
{
    std::vector<int>* loot;
    bool canceled;

public:
    Collection(){loot = nullptr; canceled = false;}
    void init(std::vector<int>* lootptr);
    void getInput(const unsigned char* keys, const unsigned char* oldKeys);
    void draw(SpriteBatcher& pics);
    bool isCanceled(){return canceled;}
    void reset(){canceled = false;}
};
