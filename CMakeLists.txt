cmake_minimum_required(VERSION 3.27)

# Название проекта
project(MyProject VERSION 0.0.01 LANGUAGES C CXX)

# Установка C++ стандарта
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указание адреса компилятора, если он не обнаружен автоматически  
if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")
endif()

if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe")
endif()

# Установка директорий для сборки
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/archive)

# Найти пакет OpenGL
find_package(OpenGL REQUIRED)
# Подключить SFML
find_package(SFML COMPONENTS graphics window system REQUIRED)
# Подключить GLM
find_package(glm REQUIRED)
# Подключить GLEW
find_package(GLEW REQUIRED)

# Использовать FetchContent для загрузки и сборки GLFW
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        latest
)
FetchContent_MakeAvailable(glfw)

# Добавить подкаталоги
add_subdirectory(Src/Engine)
 #add_subdirectory(Src/Game)



# Связать ваш проект с библиотеками
target_link_libraries(${PROJECT_NAME} 
    OpenGL::GL
    sfml-graphics sfml-window sfml-system
    glm::glm
    GLEW::GLEW
    glfw
)

# Копировать файлы данных в выходную директорию
file(COPY ${CMAKE_SOURCE_DIR}/Src/Engine/Data DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Установки для тестирования и упаковки
include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
