cmake_minimum_required(VERSION 3.30)
project(EnoddGameEngine VERSION 0.1.0 LANGUAGES C CXX)


# Устанавливаем версию C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем OpenGL и GLFW
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(EnoddGameEngine 
        Src/main.cpp
        
        
        )
# Подключаем библиотеки к проекту
target_link_libraries(EnoddGameEngine OpenGL::GL glfw)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
