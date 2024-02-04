cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

set (CMAKE_CXX_STANDARD 17)
set(PROJECT_NAME GameEngine)

project(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)


# Загрузка и подключение GLFW
FetchContent_Declare(GLFW
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8)
FetchContent_MakeAvailable(GLFW)

# Загрузка и подключение SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1)
FetchContent_MakeAvailable(SFML)

# Загрузка и подключение GLEW
add_subdirectory(src/Libs/glew-2.2.0/build/cmake)

# Загрузка и подключение GLAD
add_subdirectory(src/Libs/glad)


add_subdirectory(src/Libs/glm)
# Добавление цели "GameEngine"
add_executable(${PROJECT_NAME}
    src/main.cpp

    #src/GameEngine/Engine.cpp
    #src/GameEngine/Engine.h

    # ... другие файлы
)

# Связывание SFML с проектом
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics)

# Связывание GLFW с проектом
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# Связывание библиотек с проектом
# подключение glfw
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
# подключение glad
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
# подключение glm
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
# подключкние glew

target_link_libraries(${PROJECT_NAME} PRIVATE glew)
# подключение Open GL
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)


# Добавление директорий с заголовочными файлами
target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_SOURCE_DIR}/include)


# Установка директории для сборки проекта
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)