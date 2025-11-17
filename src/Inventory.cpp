#include "Inventory.h"
#include "ActionKeys.h"
#include "SpriteBatcher.h"
#include "Item.h"
#include "Consts.h"
#include "GameData.h"
#include "gui/Text.h"
#include "TouchData.h"

void Inventory::draw(SpriteBatcher& pics, DArray<int>& loot, GameData& gd)
{

    pics.draw(-1, 20, 80, false, 0, 600, 200, 0, COLOR(1,1,1,0.5f), COLOR(1,1,1,0.5f));
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


    if (loot.count())
    {
        GameDescription* game = gd.getGame(loot[state] - ITEM_GAME_NINJA_MAN);
        pics.draw(11, 240, 100, loot[state] - ITEM_GAME_NINJA_MAN, false, 2, 2);
        WriteShadedText(350, 100, pics, PICTURE_FONT, game->name, 0.8f, 0.8f);
        WriteSmallText(310, 150, pics, PICTURE_FONT_SMALL, game->description, 1, 1, COLOR(0,0,0,1), COLOR(0,0,0,1));
        WriteSmallText(310, 170, pics, PICTURE_FONT_SMALL, game->description1, 1, 1, COLOR(0,0,0,1), COLOR(0,0,0,1));
        WriteSmallText(310, 220, pics, PICTURE_FONT_SMALL, game->effect, 1, 1, COLOR(0,0,0,1), COLOR(0,0,0,1));
        WriteSmallText(310, 240, pics, PICTURE_FONT_SMALL, game->effect1, 1, 1, COLOR(0,0,0,1), COLOR(0,0,0,1));
    }

}

void Inventory::getInput(const unsigned char* keys,
                         const unsigned char* oldKeys,
                         TouchData& touches,
                         DArray<int>& loot)
{

    if (!touches.up.empty())
    {
        if (touches.up[0].x > 30 && touches.up[0].x < 30 + 6 * 34 && touches.up[0].y > 120 && touches.up[0].y < 120 + 34 * 3)
        {
            state = ((touches.up[0].y - 120 / 34) * 6) + (touches.up[0].x - 30) / 34;
            state = (state >= (int)loot.count()) ? loot.count() - 1 : state;
            selected = true;
        }
    }

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
