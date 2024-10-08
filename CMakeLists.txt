cmake_minimum_required(VERSION 3.29)
project(EnoddGameEngine VERSION 0.1.0 LANGUAGES C CXX)

# Устанавливаем версию C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Укажите путь к vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "D:/CppLibs/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Подключаем библиотеки и компоненты
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)


# Создаем исполняемый файл
add_executable(EnoddGameEngine 
    Src/main.cpp
)

# Подключаем библиотеки к проекту
target_link_libraries(EnoddGameEngine 
                PRIVATE 
                        OpenGL::GL
                        glfw
)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
