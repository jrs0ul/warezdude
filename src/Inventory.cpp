#include "Inventory.h"
#include "ActionKeys.h"
#include "TextureLoader.h"
#include "Item.h"
#include "Consts.h"
#include "gui/Text.h"

void Inventory::draw(PicsContainer& pics, DArray<int>& loot)
{

    pics.draw(-1, 20, 80, false, 0, 500, 200, 0, COLOR(1,1,1,0.5f), COLOR(1,1,1,0.5f));
    WriteShadedText( 30, 90, pics, PICTURE_FONT, "Loot:");


    int counter = 0;

    int posY = 120;

    for (unsigned i = 0; i < loot.count(); ++i)
    {

        const int posX = 30 + counter * 34;

        const COLOR c = (i == (unsigned)state) ? COLOR(1, 0, 0, 0.5f) : COLOR(1, 1, 1, 0.5f);

        pics.draw(-1, posX, posY, 0, false, 32, 32, 0, c, c);
        pics.draw(11, posX, posY, loot[i] - ITEM_GAME_NINJA_MAN, false);

        ++counter;

        if (counter >= 6)
        {
            posY += 34;
            counter = 0;
        }
    }
}

void Inventory::getInput(const unsigned char* keys, const unsigned char* oldKeys, DArray<int>& loot)
{
    if (keys[ACTION_BACK] && !oldKeys[ACTION_BACK])
    {
        canceled = true;
    }

    if (keys[ACTION_RIGHT] && !oldKeys[ACTION_RIGHT])
    {
        --state;

        if (state < 0)
        {
            state = loot.count() - 1;
        }
    }

    if (keys[ACTION_LEFT] && !oldKeys[ACTION_LEFT])
    {
        ++state;

        if (state >= (int)loot.count() )
        {
            state = 0;
        }
    }

    if (keys[ACTION_OPEN] && !oldKeys[ACTION_OPEN])
    {
        selected = true;
    }



}
