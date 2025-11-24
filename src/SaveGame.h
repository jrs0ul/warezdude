#pragma once

#include <vector>


class SaveGame
{
public:
    static void save(const char* documentPath, std::vector<int>* stash);
    static bool load(const char* documentPath, std::vector<int>* stash);
};
