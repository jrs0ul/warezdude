#pragma once

#include <disarray/gui/BasicUiControl.h>
#include <vector>

class SpriteBatcher;
class GameData;
struct TouchData;

class Inventory : public BasicControl
{
    int state;
    bool selected;
    bool canceled;
public:
    Inventory(){state = 0; canceled = false; selected = false;}
    void draw(SpriteBatcher& pics, std::vector<int>& loot, GameData& gd);
    void getInput(const unsigned char* keys,
                  const unsigned char* oldKeys,
                  TouchData& touches,
                  std::vector<int>& loot);
    void reset(){canceled = false; selected = false;}
    unsigned getSelected(){return state;}
    bool isSelected(){return selected;}
    bool isCanceled(){return canceled;}
};
