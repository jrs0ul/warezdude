# Codename: Warezdude

![gameplay](wd.gif)

## Game features
- 8 randomly generated levels. 
- Network-based multiplayer mode (UDP protocol).
- Collectible game cartridges that act as special abilities for the main character.
- Very annoying music. :-)

## Controls

**Arrows** or **W,A,S,D** - hero movement\
**Mouse** - aim\
**CTRL** or **Left Mouse button** - shoots\
**TAB** - Mini map\
**I** - inventory\
**SPACE** or **RETURN** - opens/closes doors

Modern game console controllers are supported as well.

Watch out for the blue monsters, they can eat items. To recover the item you have to kill the monster.

## Building from source

The game can be built using CMake (run the cmake_build.sh on linux), scons or with Visual Studio.

In order to compile you will need: OpenAL, SDL2, libogg/vorbis

To build the tests you will need GoogleTest.
