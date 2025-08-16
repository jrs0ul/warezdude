#pragma once

#include "gui/BasicUiControl.h"
#include "DArray.h"

class PicsContainer;

class Collection: public BasicControl
{
    DArray<int>* loot;
    bool canceled;

public:
    Collection(){loot = nullptr; canceled = false;}
    void init(DArray<int>* lootptr);
    void getInput(const unsigned char* keys, const unsigned char* oldKeys);
    void draw(PicsContainer& pics);
    bool isCanceled(){return canceled;}
    void reset(){canceled = false;}
};
