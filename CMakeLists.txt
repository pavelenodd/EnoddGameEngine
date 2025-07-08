cmake_minimum_required(VERSION 3.30.0 FATAL_ERROR)

set(PROJECT_NAME EnoddGameEngine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/Engine)

project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES C CXX)

find_package(Vulkan REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log log_setup thread filesystem)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(EnTT REQUIRED)
find_library(SFML sfml-graphics sfml-window sfml-system)

add_executable(${PROJECT_NAME}
    Src/main.cpp

    Src/game_loop.h

    # Managers
    Src/Managers/Engine/manager_base.h

    Src/Managers/Engine/manager_inputs.h

    Src/Managers/Engine/manager_physics.h

    Src/Managers/Engine/manager_resource.h
    Src/Managers/Engine/Items/audio.h
    Src/Managers/Engine/Items/font.h
    Src/Managers/Engine/Items/model.h
    Src/Managers/Engine/Items/texture.h

    Src/Managers/Engine/manager_scene.h
    Src/Managers/Engine/manager_render.h
    Src/Managers/Engine/manager_entity.h

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
    ${SFML_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SFML_LIBRARIES}

    Vulkan::Vulkan

    Boost::system
    Boost::log
    Boost::thread
    Boost::filesystem

    nlohmann_json::nlohmann_json

    EnTT::EnTT
)
# === Копирование файлов в папку build/Engine ================================
add_custom_command( # копировать `Src/Settings` в папку build/Engine
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/build/Engine/Settings
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Src/Settings
    ${CMAKE_SOURCE_DIR}/build/Engine/Settings
)

