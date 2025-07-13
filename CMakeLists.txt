cmake_minimum_required(VERSION 3.1)

project(WarezDude3 LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(easterncity
                    src/mainer.cpp
                    src/Game.cpp
                    )

target_link_libraries(easterncity 
                        SDL2
                        GL
                        openal
                        vorbisfile
                        ogg
                        pthread
                        curl
                     )

