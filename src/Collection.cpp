#include "Collection.h"
#include "TextureLoader.h"
#include "Item.h"
#include "ActionKeys.h"


void Collection::init(DArray<int>* lootptr)
{
    loot = lootptr;
}

void Collection::getInput(const unsigned char* keys, const unsigned char* oldKeys)
{
    if (keys[ACTION_BACK] && !oldKeys[ACTION_BACK])
    {
        canceled = true;
    }

}

void Collection::draw(PicsContainer& pics)
{
    for (int i = ITEM_GAME_NINJA_MAN; i < ITEM_GAME_TARGET_REBEL + 1; ++i)
    {

        bool found = false;

        for (unsigned a = 0; a < loot->count(); ++a)
        {
            if ((*loot)[a] == i)
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            pics.draw(11, (i - ITEM_GAME_NINJA_MAN) * 32, 150, i - ITEM_GAME_NINJA_MAN, false);
        }
    }
}
