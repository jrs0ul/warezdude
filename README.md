![gameplay](wd.gif)

##Game features:

8 random generated levels. 
Coop multiplayer or deathmatch (UDP protocol).
Collectable game cartridges that act as special abilities for the main character.

##Controls:

**Arrows** or **W,A,S,D** - hero movement\
**Mouse** - aim\
**CTRL** or **Left Mouse button** - shoots\
**TAB** - Mini map\
**I** - inventory\
**SPACE** or **RETURN** - opens/closes doors

Modern game console controllers are supported as well.

Watch out for the blue monsters, they can eat items. To recover the item you have to kill the monster.

##building from source

The game could be built using CMake (run the cmake_build.sh on linux) or with Visual Studio.

In order to compile you will need: OpenAL, SDL2, libogg/vorbis

To build the tests you will need GoogleTest.
