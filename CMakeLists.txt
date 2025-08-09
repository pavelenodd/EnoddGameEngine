cmake_minimum_required(VERSION 3.30.0 FATAL_ERROR)

set(PROJECT_NAME EnoddGameEngine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/Engine)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(RELEASE)
endif()

project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

find_package(Vulkan REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log log_setup thread filesystem)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(assimp REQUIRED)
find_package(EnTT REQUIRED)
find_package(SFML 3 REQUIRED COMPONENTS System)
find_package(bgfx CONFIG REQUIRED)

set(GAME_SRC 
    Game/snake_game.h
    Game/snake_game.cpp
)
set(ENGINE_SRC
    Src/main.cpp

    Src/game_loop.h

    # Managers
    Src/Managers/manager_base.h

    Src/Managers/manager_inputs.h

    Src/Managers/manager_physics.h

    Src/Managers/manager_resource.h
    Src/Managers/Items/audio.h
    Src/Managers/Items/font.h
    Src/Managers/Items/model.h
    Src/Managers/Items/texture.h

    Src/Managers/manager_scene.h
    Src/Managers/manager_render.h
    Src/Managers/manager_entity.h

    # EngineLogging
    Src/EngineError/engine_logging.h

    # DATA
    Src/EngineData/engine_data.h

    # Tools
    Src/Tools/delegate.h
    Src/Tools/interface.h
    Src/Tools/json_reader.h
)
set(TEST_ENGINE_SRC
    Tests/test_manager_inputs.h
    Tests/test_manager_inputs.cpp
)
set(TEST_GAME_SRC
)

add_executable(${PROJECT_NAME}
    ${GAME_SRC}
    ${ENGINE_SRC}

    # Tests
    ${TEST_ENGINE_SRC}
    ${TEST_GAME_SRC}
    
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

    bgfx::bgfx
    assimp::assimp

    nlohmann_json::nlohmann_json

    EnTT::EnTT
    
    sfml-graphics
    sfml-window
    sfml-system
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
add_custom_command( # копировать `Src/Resources` в папку build/Engine
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/build/Engine/Resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Src/Resources
    ${CMAKE_SOURCE_DIR}/build/Engine/Resources
)
# =========== ТЕСТЫ =============
add_test(NAME SmokeTest_Basic
    COMMAND ${PROJECT_NAME})
set_tests_properties(SmokeTest_Basic PROPERTIES
    TIMEOUT 5   
    LABELS "smoke"
)

# Тест на утечки памяти (с Valgrind если доступен)
find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE)
    add_test(NAME MemoryTest_Valgrind
        COMMAND ${VALGRIND_EXECUTABLE} 
        --leak-check=full 
        --error-exitcode=1 
        --show-leak-kinds=all
        $<TARGET_FILE:${PROJECT_NAME}>)
    set_tests_properties(MemoryTest_Valgrind PROPERTIES
        TIMEOUT 300  
        LABELS "memory"
    )
endif()
