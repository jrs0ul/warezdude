#pragma once

#include "DArray.h"


class SaveGame
{
public:
    static void save(const char* documentPath, DArray<int>* stash);
    static bool load(const char* documentPath, DArray<int>* stash);
};
