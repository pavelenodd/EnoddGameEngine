cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

set (CMAKE_CXX_STANDARD 17)
set(PROJECT_NAME GameEngine)

project(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# Загрузка и подключение SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1)
FetchContent_MakeAvailable(SFML)

# Загрузка и подключение GLFW
FetchContent_Declare(GLFW
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8)
FetchContent_MakeAvailable(GLFW)

# Добавление цели "GameEngine"
add_executable(${PROJECT_NAME}
    src/main.cpp
)

# Указание компонентов SFML, которые вы используете
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics)

# Указание компонентов GLFW, которые вы используете
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<BOOL:${ARCH_64BITS}>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM)
endif()

# Добавление директорий с заголовочными файлами
target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_SOURCE_DIR}/include)

# Установка директории для сборки проекта
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
