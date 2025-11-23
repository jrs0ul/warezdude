cmake_minimum_required(VERSION 3.1)

project(CQEditor LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})


add_executable(CQEditor
               src/main.cpp
               src/CIniFile.cpp
               ../src/map.cpp
               ../src/MapGenerator.cpp
               ../src/SpriteBatcher.cpp
               ../src/Image.cpp
               ../src/Dude.cpp
               ../src/Particles2D.cpp
               ../src/bullet.cpp
               ../src/Useful.cpp
               ../src/gui/Text.cpp
               ../src/Vectors.cpp
               ../src/Shader.cpp
               ../src/SDLVideo.cpp
               ../src/VulkanVideo.cpp
                      ../src/Xml.cpp
                      ../src/BulletContainer.cpp
                      ../src/OSTools.cpp
                      ../src/Decal.cpp
                      src/Button.cpp
                      src/Utils.cpp
                      ../src/Matrix.cpp
                      ../src/ShaderProgram.cpp
                      ../src/Extensions.cpp
            )
target_link_libraries(CQEditor 
                        SDL2
                        GL
                        openal
                        vorbisfile
                        vulkan
                        ogg
                     )
