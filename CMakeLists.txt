cmake_minimum_required(VERSION 3.1)

project(CartridgeQuest LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

set(VULKAN_VERSION "vulkan1.0")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include_directories(CartridgeQuest PRIVATE "src/disarray/")
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


add_executable(CartridgeQuest
                    src/Inventory.cpp
                    src/Intro.cpp
                    src/SaveGame.cpp
                    src/bullet.cpp
                    src/BulletContainer.cpp
                    src/GameData.cpp
                    src/Collection.cpp
                    src/map.cpp
                    src/maplist.cpp
                    src/Decal.cpp
                    src/MapGenerator.cpp
                    src/Dude.cpp
                    src/Game.cpp
                    src/mainer.cpp
                    )

                add_library(disarray
                    src/disarray/gui/SelectMenu.cpp
                    src/disarray/OSTools.cpp
                    src/disarray/Particles2D.cpp
                    src/disarray/gui/Text.cpp
                    src/disarray/gui/Slider.cpp
                    src/disarray/gui/EditBox.cpp
                    src/disarray/SysConfig.cpp
                    src/disarray/network/Socket.cpp
                    src/disarray/network/Client.cpp
                    src/disarray/network/Server.cpp
                    src/disarray/Vectors.cpp
                    src/disarray/Xml.cpp
                    src/disarray/Matrix.cpp
                    src/disarray/Useful.cpp
                    src/disarray/SpriteBatcher.cpp
                    src/disarray/Image.cpp
                    src/disarray/RenderTexture.cpp
                    src/disarray/VulkanVideo.cpp
                    src/disarray/SDLVideo.cpp
                    src/disarray/Extensions.cpp
                    src/disarray/ShaderProgram.cpp
                    src/disarray/Shader.cpp
                    src/disarray/audio/OggStream.cpp
                    src/disarray/audio/SoundSystem.cpp

                    )

                target_link_libraries(disarray
                        SDL2
                        GL
                        openal
                        vorbisfile
                        ogg
                        vulkan
                    )

                target_link_libraries(CartridgeQuest
                        disarray
                     )

add_subdirectory(editor)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/shaders/justcolor_vert.spv
    COMMAND glslangValidator ${CMAKE_SOURCE_DIR}/shaders/vk_justcolor.vert --target-env ${VULKAN_VERSION} -o ${CMAKE_SOURCE_DIR}/shaders/justcolor_vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/vk_justcolor.vert
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/shaders/justcolor_frag.spv
    COMMAND glslangValidator ${CMAKE_SOURCE_DIR}/shaders/vk_justcolor.frag --target-env ${VULKAN_VERSION} -o ${CMAKE_SOURCE_DIR}/shaders/justcolor_frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/vk_justcolor.frag
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/shaders/default_vert.spv
    COMMAND glslangValidator ${CMAKE_SOURCE_DIR}/shaders/vk_default.vert --target-env ${VULKAN_VERSION} -o ${CMAKE_SOURCE_DIR}/shaders/default_vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/vk_default.vert
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/shaders/default_frag.spv
    COMMAND glslangValidator ${CMAKE_SOURCE_DIR}/shaders/vk_default.frag --target-env ${VULKAN_VERSION} -o ${CMAKE_SOURCE_DIR}/shaders/default_frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/vk_default.frag
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/shaders/filmGrain_vert.spv
    COMMAND glslangValidator ${CMAKE_SOURCE_DIR}/shaders/vk_filmGrain.vert --target-env ${VULKAN_VERSION} -o ${CMAKE_SOURCE_DIR}/shaders/filmGrain_vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/vk_filmGrain.vert
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/shaders/filmGrain_frag.spv
    COMMAND glslangValidator ${CMAKE_SOURCE_DIR}/shaders/vk_filmGrain.frag --target-env ${VULKAN_VERSION} -o ${CMAKE_SOURCE_DIR}/shaders/filmGrain_frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/vk_filmGrain.frag
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/shaders/scanlines_vert.spv
    COMMAND glslangValidator ${CMAKE_SOURCE_DIR}/shaders/vk_scanlines.vert --target-env ${VULKAN_VERSION} -o ${CMAKE_SOURCE_DIR}/shaders/scanlines_vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/vk_scanlines.vert
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/shaders/scanlines_frag.spv
    COMMAND glslangValidator ${CMAKE_SOURCE_DIR}/shaders/vk_scanlines.frag --target-env ${VULKAN_VERSION} -o ${CMAKE_SOURCE_DIR}/shaders/scanlines_frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/vk_scanlines.frag
)





add_custom_target(CompileShaders ALL DEPENDS
    ${CMAKE_SOURCE_DIR}/shaders/justcolor_vert.spv
    ${CMAKE_SOURCE_DIR}/shaders/justcolor_frag.spv
    ${CMAKE_SOURCE_DIR}/shaders/default_vert.spv
    ${CMAKE_SOURCE_DIR}/shaders/default_frag.spv
    ${CMAKE_SOURCE_DIR}/shaders/filmGrain_vert.spv
    ${CMAKE_SOURCE_DIR}/shaders/filmGrain_frag.spv
    ${CMAKE_SOURCE_DIR}/shaders/scanlines_vert.spv
    ${CMAKE_SOURCE_DIR}/shaders/scanlines_frag.spv
)


