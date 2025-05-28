cmake_minimum_required(VERSION 3.30.0 FATAL_ERROR)

include(FetchContent)

set(PROJECT_NAME EnoddGameEngine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES C CXX)

find_package(Vulkan REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log log_setup thread filesystem)
find_library(SFML sfml-Graphics sfml-Window sfml-System)
find_package(nlohmann_json REQUIRED)

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
    Src/Managers/manager_base.h 

    Src/Managers/manager_inputs.h

    Src/Managers/manager_physics.h

    Src/Managers/manager_resourse.h
        Src/Managers/Items/audio.h
        Src/Managers/Items/font.h
        Src/Managers/Items/model.h
        Src/Managers/Items/texture.h

    Src/Managers/manager_scene.h
    Src/Managers/manager_render.h
# EngineLogging
    Src/EngineError/engine_error.h
    Src/EngineError/engine_logging.h
# DATA
    Src/EngineData/engine_data.h
# Tools
    Src/Tools/delegate.h 
    Src/Tools/interface.h
    Src/Tools/json_reader.h
    )

    target_include_directories(${PROJECT_NAME} PRIVATE  
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Managers
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/EngineError
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
# Post-build: копируем исполняемый файл в Engine/Libs
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory
          ${CMAKE_SOURCE_DIR}/Engine/Libs
  COMMAND ${CMAKE_COMMAND} -E copy
          $<TARGET_FILE:${PROJECT_NAME}>
          ${CMAKE_SOURCE_DIR}/Engine/Libs
)

# Post-build: копируем всю папку Src/Settings в Engine/Settings
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/Src/Settings
          ${CMAKE_SOURCE_DIR}/Engine/Settings
)
# --------------------------------------------------------------------


include(CTest)
enable_testing()
