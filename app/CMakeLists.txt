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

include_directories("../src/disarray/")
include_directories("${OPENAL_DIR}/include")
include_directories("${OGG_VORBIS_DIR}/inc")

add_library(disarray SHARED
            ${DISARRAY_DIR}VulkanVideo.cpp
            ${DISARRAY_DIR}SpriteBatcher.cpp
            ${DISARRAY_DIR}audio/SoundSystem.cpp
            ${DISARRAY_DIR}audio/OggStream.cpp
            ${DISARRAY_DIR}gui/SelectMenu.cpp
            ${DISARRAY_DIR}OSTools.cpp
            ${DISARRAY_DIR}Particles2D.cpp
            ${DISARRAY_DIR}gui/Text.cpp
            ${DISARRAY_DIR}gui/Slider.cpp
            ${DISARRAY_DIR}ShaderProgram.cpp
            ${DISARRAY_DIR}Shader.cpp
            ${DISARRAY_DIR}network/Socket.cpp
            ${DISARRAY_DIR}network/Client.cpp
            ${DISARRAY_DIR}network/Server.cpp
            ${DISARRAY_DIR}Vectors.cpp
            ${DISARRAY_DIR}gui/EditBox.cpp
            ${DISARRAY_DIR}SysConfig.cpp
            ${DISARRAY_DIR}Xml.cpp
            ${DISARRAY_DIR}Matrix.cpp
            ${DISARRAY_DIR}Useful.cpp
            ${DISARRAY_DIR}AndroidFile.cpp
            ${DISARRAY_DIR}SpriteBatcher.cpp
            ${DISARRAY_DIR}Image.cpp
            ${DISARRAY_DIR}RenderTexture.cpp
           )

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

add_library(openal-soft SHARED IMPORTED)
set_target_properties(openal-soft PROPERTIES IMPORTED_LOCATION
        "${OPENAL_DIR}/${ANDROID_ABI}/libopenal.so")
add_library(ogg SHARED IMPORTED)
set_target_properties(ogg PROPERTIES IMPORTED_LOCATION
        "${OGG_VORBIS_DIR}/lib/${ANDROID_ABI}/libogg.a")
add_library(vorbis SHARED IMPORTED)
set_target_properties(vorbis PROPERTIES IMPORTED_LOCATION
        "${OGG_VORBIS_DIR}/lib/${ANDROID_ABI}/libvorbis.a")


set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u \
    Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")

target_link_libraries(disarray PUBLIC
                        android
                        log
                        vulkan
                        openal-soft
                        EGL
                        GLESv2
                        ogg
                        vorbis
                     )

target_link_libraries(game PUBLIC
                       disarray
                       android
                       game-activity::game-activity_static
                       log )
