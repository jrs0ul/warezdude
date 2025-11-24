#include "SaveGame.h"
#include <cstdio>

void SaveGame::save(const char* documentPath, std::vector<int>* stash)
{
    FILE * f = NULL;

    char name[512];
    sprintf(name, "%s/save.sav", documentPath);
    f = fopen(name, "wb+");
    unsigned long stashSize = stash->size();
    fwrite(&stashSize, sizeof(unsigned long), 1, f);
    fwrite(stash->data(), sizeof(int), stashSize, f);
}

bool SaveGame::load(const char* documentPath, std::vector<int>* stash)
{
    FILE * f = NULL;

    char name[512];
    sprintf(name, "%s/save.sav", documentPath);
    f = fopen(name, "rb");

    if (!f)
    {
        return false;
    }

    unsigned long stashSize = 0;
    fread(&stashSize, sizeof(unsigned long), 1, f);

    for (unsigned long i = 0; i < stashSize; ++i)
    {
        int item = 0;
        fread(&item, sizeof(int), 1, f);
        stash->push_back(item);
    }

    return true;
}
