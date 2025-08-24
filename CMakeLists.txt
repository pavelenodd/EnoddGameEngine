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

# ====== Дополнительные опции для проверки качества кода ======
option(ENABLE_WARNINGS "Включить расширенные предупреждения" ON)
option(ENABLE_WARNINGS_AS_ERRORS "Трактовать предупреждения как ошибки" OFF)
option(ENABLE_ANALYZER "Включить встроенный статический анализатор (-fanalyzer GCC)" OFF)
option(ENABLE_ASAN "Включить AddressSanitizer" OFF)
option(ENABLE_UBSAN "Включить UndefinedBehaviorSanitizer" OFF)
option(ENABLE_TSAN "Включить ThreadSanitizer (несовместим с ASAN/UBSAN)" OFF)

set(WARNING_FLAGS "")
if(ENABLE_WARNINGS)
    list(APPEND WARNING_FLAGS
        -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow
        -Wdouble-promotion -Wold-style-cast -Wnon-virtual-dtor -Woverloaded-virtual
        -Wnull-dereference
    )
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND WARNING_FLAGS -Wlogical-op -Wuseless-cast -Wduplicated-cond -Wduplicated-branches)
    endif()
    if(ENABLE_WARNINGS_AS_ERRORS)
        list(APPEND WARNING_FLAGS -Werror)
    endif()
endif()

set(SAN_FLAGS "")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    if(ENABLE_TSAN)
        list(APPEND SAN_FLAGS -fsanitize=thread)
    else()
        if(ENABLE_ASAN)
            list(APPEND SAN_FLAGS -fsanitize=address)
        endif()
        if(ENABLE_UBSAN)
            list(APPEND SAN_FLAGS -fsanitize=undefined)
        endif()
    endif()
    if(SAN_FLAGS)
        # Для лучшей диагностики стеков
        list(APPEND SAN_FLAGS -fno-omit-frame-pointer)
    endif()
endif()

if(ENABLE_ANALYZER AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND WARNING_FLAGS -fanalyzer)
endif()

include(CTest)
enable_testing()

find_package(Vulkan REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log log_setup thread filesystem)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(assimp REQUIRED)
find_package(EnTT REQUIRED)
find_package(SFML 3 REQUIRED COMPONENTS System)
find_package(bgfx CONFIG REQUIRED)
find_package(glfw3  REQUIRED)
find_package(GTest REQUIRED)

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
    Tests/test_manager_render.h
    Tests/test_manager_resources.h
    Tests/test_manager_scene.h
    Tests/test_manager_settings.h
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

if(WARNING_FLAGS)
    target_compile_options(${PROJECT_NAME} PRIVATE ${WARNING_FLAGS})
endif()
if(SAN_FLAGS)
    target_compile_options(${PROJECT_NAME} PRIVATE ${SAN_FLAGS})
    target_link_options(${PROJECT_NAME} PRIVATE ${SAN_FLAGS})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Managers
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/EngineError
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Tools
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/EngineData
    ${CMAKE_CURRENT_SOURCE_DIR}/Tests
    ${CMAKE_CURRENT_SOURCE_DIR}/Game
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
    
    glfw


    sfml-graphics
    sfml-window
    sfml-system


    GTest::gtest
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
# Smoke 
add_test(NAME SmokeTest_Basic
    COMMAND ${PROJECT_NAME})
set_tests_properties(SmokeTest_Basic PROPERTIES
    TIMEOUT 5
    LABELS "smoke"
)

# GoogleTest :
## ManagerScene
add_test(NAME ManagerScene_GTests
    COMMAND ${PROJECT_NAME} --run-tests --gtest_filter=SceneTest.*
)
set_tests_properties(ManagerScene_GTests PROPERTIES
    TIMEOUT 60
    LABELS "gtest;manager_scene"
)

# Valgrind 
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

# Тест: синтаксическая проверка отдельных заголовков (без линковки)
add_test(NAME SyntaxCheck_manager_settings
    COMMAND ${CMAKE_CXX_COMPILER} -std=c++${CMAKE_CXX_STANDARD} -fsyntax-only
            $<$<BOOL:${WARNING_FLAGS}>:${WARNING_FLAGS}>
            ${CMAKE_SOURCE_DIR}/Src/Managers/manager_settings.h)
set_tests_properties(SyntaxCheck_manager_settings PROPERTIES
    LABELS "syntax"
    PASS_REGULAR_EXPRESSION ""
)
