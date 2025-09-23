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
option(ENABLE_ANALYZER "Включить встроенный статический анализатор (-fanalyzer GCC)" OFF)
option(ENABLE_ASAN "Включить AddressSanitizer" OFF)
option(ENABLE_UBSAN "Включить UndefinedBehaviorSanitizer" OFF)
option(ENABLE_TSAN "Включить ThreadSanitizer (несовместим с ASAN/UBSAN)" OFF)

# === Платформенные опции для GLFW/native access (Linux) =====================
# РОВНО один стек: X11 (по умолчанию) или Wayland. Для Vulkan ничего больше не нужно.
option(ENODD_GLFW_WAYLAND "Use Wayland native access instead of X11 (Linux only)" OFF)


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

# === Пакеты ==================================================================
# Vulkan + компоненты: берём shaderc и glslc через FindVulkan (так надёжнее в Arch)
find_package(Vulkan REQUIRED COMPONENTS shaderc_combined glslc)
find_package(Boost REQUIRED COMPONENTS system log log_setup thread filesystem)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(assimp REQUIRED)
find_package(EnTT REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)  
find_path(STB_INCLUDE_DIR stb/stb_image.h)

# ! это пример по которому надо ориентироваться на подключение библиотек
# ! если библиотека не найдена, то либо ставим её в систему, либо используем FetchContent
if (NOT STB_INCLUDE_DIR)
  message(FATAL_ERROR "stb headers not found. Install libstb-dev.")
endif()
add_library(stb INTERFACE)
add_library(stb::stb ALIAS stb)
target_include_directories(stb INTERFACE ${STB_INCLUDE_DIR})
message(STATUS "stb include dir: ${STB_INCLUDE_DIR}")



find_package(volk QUIET)     # даёт volk::volk
if (volk_FOUND)
  add_compile_definitions(EGE_USE_VOLK)
endif()

find_path(VMA_INCLUDE_DIRS "vk_mem_alloc.h")
if (NOT VMA_INCLUDE_DIRS)
  message(FATAL_ERROR "VMA not found. Install 'vulkan-memory-allocator' or use FetchContent.")
endif()

# Тесты
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
    
    Src/Managers/manager_settings.h
    Src/Managers/manager_settings.cpp
    
    Src/Managers/manager_physics.h

    Src/Managers/manager_resource.h
    Src/Managers/manager_resource.cpp

    Src/Managers/Items/audio.h
    Src/Managers/Items/font.h
    Src/Managers/Items/model.h
    Src/Managers/Items/texture.h

    Src/Managers/manager_scene.h
    Src/Managers/manager_scene.cpp

    Src/Managers/manager_render.h
    Src/Managers/manager_render.cpp
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

# Compile shaders
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/Src/Assets/Shaders/simple.vert.spv
    COMMAND Vulkan::glslc ${CMAKE_SOURCE_DIR}/Src/Assets/Shaders/simple.vert -o ${CMAKE_SOURCE_DIR}/Src/Assets/Shaders/simple.vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/Src/Assets/Shaders/simple.vert
    COMMENT "Compiling vertex shader"
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/Src/Assets/Shaders/simple.frag.spv
    COMMAND Vulkan::glslc ${CMAKE_SOURCE_DIR}/Src/Assets/Shaders/simple.frag -o ${CMAKE_SOURCE_DIR}/Src/Assets/Shaders/simple.frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/Src/Assets/Shaders/simple.frag
    COMMENT "Compiling fragment shader"
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
    ${CMAKE_SOURCE_DIR}/Src/Assets/Shaders/simple.vert.spv
    ${CMAKE_SOURCE_DIR}/Src/Assets/Shaders/simple.frag.spv
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
# === Макросы для GLFW + Vulkan ==============================================
target_compile_definitions(EnoddGameEngine PRIVATE
  GLFW_INCLUDE_VULKAN       # заставляет glfw3.h подключить vulkan/vulkan.h
  GLFW_INCLUDE_NONE         # не тянуть OpenGL заголовки
)

# === Линки ==================================================================
target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan

    Boost::system
    Boost::log
    Boost::thread
    Boost::filesystem

    assimp::assimp

    nlohmann_json::nlohmann_json
    EnTT::EnTT
    stb::stb
    glfw
    glm::glm

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
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/build/Engine/Assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Src/Assets
            ${CMAKE_SOURCE_DIR}/build/Engine/Assets
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
