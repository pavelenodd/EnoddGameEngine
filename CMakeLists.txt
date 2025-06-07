cmake_minimum_required(VERSION 3.30.0 FATAL_ERROR)

include(FetchContent)

set(PROJECT_NAME EnoddGameEngine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES C CXX)

# If using CMake, add this to your CMakeLists.txt to link the package:
find_package(Vulkan REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log log_setup thread filesystem)
find_package(nlohmann_json CONFIG REQUIRED)
find_library(SFML sfml-Graphics sfml-Window sfml-System)

FetchContent_Declare(
    bgfx
    GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake.git
    GIT_TAG master
)
FetchContent_MakeAvailable(bgfx)

add_executable(${PROJECT_NAME}
    Src/main.cpp

    Src/game_loop.h

    # Managers
    Src/Managers/Engine/manager_base.h

    Src/Managers/Engine/manager_inputs.h

    Src/Managers/Engine/manager_physics.h

    Src/Managers/Engine/manager_resourse.h
    Src/Managers/Engine/Items/audio.h
    Src/Managers/Engine/Items/font.h
    Src/Managers/Engine/Items/model.h
    Src/Managers/Engine/Items/texture.h

    Src/Managers/Engine/manager_scene.h
    Src/Managers/Engine/manager_render.h

    # EngineLogging
    Src/EngineError/engine_error.h
    Src/EngineError/engine_logging.h

    # DATA
    Src/EngineData/engine_data.h

    # Tools
    Src/Tools/Engine/delegate.h
    Src/Tools/Engine/interface.h
    Src/Tools/Engine/json_reader.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Managers
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/EngineError
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Tools
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/EngineData
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system

    Vulkan::Vulkan

    Boost::system
    Boost::log
    Boost::thread
    Boost::filesystem

    bgfx
    bimg
    bx

    nlohmann_json::nlohmann_json
)

# --------------------------------------------------------------------
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/Engine
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/build/Engine
    
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Src/Settings
    ${CMAKE_SOURCE_DIR}/build/Engine/Settings
)

# --------------------------------------------------------------------
include(CTest)
enable_testing()
