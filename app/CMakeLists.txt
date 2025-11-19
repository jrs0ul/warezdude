cmake_minimum_required(VERSION 3.4.1)

project(CartridgeQuest LANGUAGES CXX)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_ANDROID_KHR")

set(SRCDIR ../src/)
find_package(game-activity REQUIRED CONFIG)

set(OPENAL_DIR "/home/jrs0ul/libopenal")
set(OGG_VORBIS_DIR "/home/jrs0ul/oggvorbis")
include_directories("${OPENAL_DIR}/include")
include_directories("${OGG_VORBIS_DIR}/inc")


add_library(game SHARED

             ${SRCDIR}VulkanVideo.cpp
             ${SRCDIR}game_android.cpp
             ${SRCDIR}Game.cpp
             ${SRCDIR}SpriteBatcher.cpp
             ${SRCDIR}audio/SoundSystem.cpp
             ${SRCDIR}audio/OggStream.cpp
             ${SRCDIR}Dude.cpp
             ${SRCDIR}MapGenerator.cpp
             ${SRCDIR}map.cpp
             ${SRCDIR}bullet.cpp
             ${SRCDIR}Decal.cpp
             ${SRCDIR}Intro.cpp
             ${SRCDIR}BulletContainer.cpp
             ${SRCDIR}maplist.cpp
             ${SRCDIR}gui/SelectMenu.cpp
             ${SRCDIR}OSTools.cpp
             ${SRCDIR}Inventory.cpp
             ${SRCDIR}Particles2D.cpp
             ${SRCDIR}gui/Text.cpp
             ${SRCDIR}gui/Slider.cpp
             ${SRCDIR}gui/EditBox.cpp
             ${SRCDIR}Collection.cpp
             ${SRCDIR}GameData.cpp
             ${SRCDIR}SaveGame.cpp
             ${SRCDIR}SysConfig.cpp
             ${SRCDIR}network/Socket.cpp
             ${SRCDIR}network/Client.cpp
             ${SRCDIR}network/Server.cpp
             ${SRCDIR}Vectors.cpp
             ${SRCDIR}Xml.cpp
             ${SRCDIR}Matrix.cpp
             ${SRCDIR}Useful.cpp
             ${SRCDIR}AndroidFile.cpp
             ${SRCDIR}SpriteBatcher.cpp
             ${SRCDIR}Image.cpp
             ${SRCDIR}RenderTexture.cpp
             ${SRCDIR}ShaderProgram.cpp
             ${SRCDIR}Shader.cpp
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



target_include_directories(game PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           )
set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u \
    Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")

target_link_libraries(game PUBLIC
                       android
                       EGL
                       GLESv2
                       #OpenSLES
                       openal-soft
                       ogg
                       vorbis
                       game-activity::game-activity_static
                       vulkan
                       log )
