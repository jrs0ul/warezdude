cmake_minimum_required(VERSION 3.4.1)

project(CartridgeQuest LANGUAGES CXX)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_ANDROID_KHR")

set(SRCDIR ../src/)
set(DISARRAY_DIR ../src/disarray/)
find_package(game-activity REQUIRED CONFIG)

set(OPENAL_DIR "/home/jrs0ul/libopenal")
set(OGG_VORBIS_DIR "/home/jrs0ul/oggvorbis")
include_directories("${OPENAL_DIR}/include")
include_directories("${OGG_VORBIS_DIR}/inc")

include_directories("${PROJECT_SOURCE_DIR}/src/main/cpp/disarray/include")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/main/cpp/disarray")

add_library(game SHARED
             ${SRCDIR}game_android.cpp
             ${SRCDIR}Game.cpp
             ${SRCDIR}Dude.cpp
             ${SRCDIR}MapGenerator.cpp
             ${SRCDIR}map.cpp
             ${SRCDIR}bullet.cpp
             ${SRCDIR}Decal.cpp
             ${SRCDIR}Intro.cpp
             ${SRCDIR}BulletContainer.cpp
             ${SRCDIR}maplist.cpp
             ${SRCDIR}Inventory.cpp
             ${SRCDIR}Collection.cpp
             ${SRCDIR}GameData.cpp
             ${SRCDIR}SaveGame.cpp
            )


set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u \
    Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")


target_link_libraries(game PUBLIC
                       disarray
                       android
                       game-activity::game-activity_static
                       log )
