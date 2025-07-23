cmake_minimum_required(VERSION 3.1)

project(WarezDude3 LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(warezdude3
                    src/mainer.cpp
                    src/Dude.cpp
                    src/map.cpp
                    src/bullet.cpp
                    src/Decal.cpp
                    src/BulletContainer.cpp
                    src/maplist.cpp
                    src/SelectMenu.cpp
                    src/OSTools.cpp
                    src/gui/Text.cpp
                    src/ScroollControl.cpp
                    src/EditBox.cpp
                    src/SysConfig.cpp
                    src/CServer.cpp
                    src/CClient.cpp
                    src/Vectors.cpp
                    src/Xml.cpp
                    src/Matrix.cpp
                    src/Usefull.cpp
                    src/TextureLoader.cpp
                    src/Image.cpp
                    src/SDLVideo.cpp
                    src/Extensions.cpp
                    src/ShaderProgram.cpp
                    src/Shader.cpp
                    src/audio/OggStream.cpp
                    src/audio/SoundSystem.cpp
                    src/Game.cpp
                    )

target_link_libraries(warezdude3 
                        SDL2
                        GL
                        openal
                        vorbisfile
                        ogg
                        pthread
                        curl
                     )

