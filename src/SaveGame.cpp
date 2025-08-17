#include "SaveGame.h"

void SaveGame::save(const char* documentPath, DArray<int>* stash)
{
    FILE * f = NULL;

    char name[512];
    sprintf(name, "%s/save.sav", documentPath);
    f = fopen(name, "wb+");
    unsigned long stashSize = stash->count();
    fwrite(&stashSize, sizeof(unsigned long), 1, f);
    fwrite(stash->getData(), sizeof(int), stashSize, f);
}

bool SaveGame::load(const char* documentPath, DArray<int>* stash)
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
        stash->add(item);
    }

    return true;
}
