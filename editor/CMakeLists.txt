cmake_minimum_required(VERSION 3.1)

project(CQEditor LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

include_directories("../src/disarray/")

add_executable(CQEditor
               src/main.cpp
               src/CIniFile.cpp
               src/Button.cpp
               src/Utils.cpp
               ../src/map.cpp
               ../src/bullet.cpp
               ../src/Dude.cpp
               ../src/MapGenerator.cpp
               ../src/BulletContainer.cpp
               ../src/Decal.cpp

            )
target_link_libraries(CQEditor 
                        disarray
                     )
