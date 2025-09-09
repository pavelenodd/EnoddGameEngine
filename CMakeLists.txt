cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

set(PROJECT_NAME EnoddGameEngine)
project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES C CXX)

# === Язык/бинарники ==========================================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/Engine)

# Удобные дефайны для кода по типу сборки
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_definitions(DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(RELEASE)
endif()

# === Опции качества кода / санитайзеры ======================================
option(ENABLE_WARNINGS "Включить расширенные предупреждения" ON)
option(ENABLE_WARNINGS_AS_ERRORS "Трактовать предупреждения как ошибки" OFF)
option(ENABLE_ANALYZER "Включить встроенный статический анализатор (-fanalyzer GCC)" ON)
option(ENABLE_ASAN "Включить AddressSanitizer" ON)
option(ENABLE_UBSAN "Включить UndefinedBehaviorSanitizer" ON)
option(ENABLE_TSAN "Включить ThreadSanitizer (несовместим с ASAN/UBSAN)" ON)

# === Платформенные опции для GLFW/native access (Linux) =====================
# РОВНО один стек: X11 (по умолчанию) или Wayland. Для Vulkan ничего больше не нужно.
option(ENODD_GLFW_WAYLAND "Use Wayland native access instead of X11 (Linux only)" OFF)

# Если линкуетесь с SHARED bgfx (.so) — это влияет только на приложение (НЕ на сборку bgfx!)
option(USE_BGFX_SHARED "Link against shared bgfx" OFF)

# === Предупреждения / Санитайзеры ===========================================
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
        list(APPEND SAN_FLAGS -fno-omit-frame-pointer)
    endif()
endif()

if(ENABLE_ANALYZER AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND WARNING_FLAGS -fanalyzer)
endif()

# === Тесты ==================================================================
include(CTest)
enable_testing()

# === Пакеты =================================================================
find_package(Vulkan REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log log_setup thread filesystem)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(assimp REQUIRED)
find_package(EnTT REQUIRED)
find_package(bgfx CONFIG REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GTest REQUIRED)

# === Источники ==============================================================

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
    Src/Managers/manager_inputs.cpp

    Src/Managers/manager_physics.h

    Src/Managers/manager_resource.h
    Src/Managers/Items/audio.h
    Src/Managers/Items/font.h
    Src/Managers/Items/model.h
    Src/Managers/Items/texture.h

    Src/Managers/manager_scene.h
    Src/Managers/manager_scene.cpp

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
    ${TEST_ENGINE_SRC}
    ${TEST_GAME_SRC}
)

# === Флаги/Опции компиляции =================================================
if(WARNING_FLAGS)
    target_compile_options(${PROJECT_NAME} PRIVATE ${WARNING_FLAGS})
endif()
if(SAN_FLAGS)
    target_compile_options(${PROJECT_NAME} PRIVATE ${SAN_FLAGS})
    target_link_options(${PROJECT_NAME} PRIVATE ${SAN_FLAGS})
endif()

# === Инклуды ================================================================
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

# === Макросы для GLFW native access (Linux): РОВНО один стек =================
if(UNIX AND NOT APPLE)
    if(ENODD_GLFW_WAYLAND)
        target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_EXPOSE_NATIVE_WAYLAND)
        message(STATUS "[GLFW native] Wayland selected for Vulkan")
    else()
        target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_EXPOSE_NATIVE_X11)
        message(STATUS "[GLFW native] X11 selected for Vulkan")
    endif()
endif()

# === bgfx: если линкуемся с shared-версией, объявляем ТОЛЬКО USE ============
if(USE_BGFX_SHARED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE BGFX_SHARED_LIB_USE)
    message(STATUS "[bgfx] Linking against shared library (BGFX_SHARED_LIB_USE)")
endif()

# === Линки ==================================================================
target_link_libraries(${PROJECT_NAME} PRIVATE
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


    GTest::gtest
)

# === Копирование ресурсов ===================================================
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/build/Engine/Settings
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Src/Settings
            ${CMAKE_SOURCE_DIR}/build/Engine/Settings
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/build/Engine/Resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Src/Resources
            ${CMAKE_SOURCE_DIR}/build/Engine/Resources
)

# === Тесты ==================================================================
add_test(NAME SmokeTest_Basic
    COMMAND ${PROJECT_NAME})
set_tests_properties(SmokeTest_Basic PROPERTIES
    TIMEOUT 5
    LABELS "smoke"
)

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

# Синтакс-проверки отдельных хедеров (по желанию)
add_test(NAME SyntaxCheck_game_loop
    COMMAND ${CMAKE_CXX_COMPILER} -std=c++${CMAKE_CXX_STANDARD} -fsyntax-only
            $<$<BOOL:${WARNING_FLAGS}>:${WARNING_FLAGS}>
            ${CMAKE_SOURCE_DIR}/Src/game_loop.h)
set_tests_properties(SyntaxCheck_game_loop PROPERTIES LABELS "syntax")

add_test(NAME SyntaxCheck_manager_settings
    COMMAND ${CMAKE_CXX_COMPILER} -std=c++${CMAKE_CXX_STANDARD} -fsyntax-only
            $<$<BOOL:${WARNING_FLAGS}>:${WARNING_FLAGS}>
            ${CMAKE_SOURCE_DIR}/Src/Managers/manager_settings.h)
set_tests_properties(SyntaxCheck_manager_settings PROPERTIES LABELS "syntax")
