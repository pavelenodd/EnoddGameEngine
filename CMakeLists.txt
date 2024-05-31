cmake_minimum_required(VERSION 3.27)

# Название проекта
project(MyProject ${PROJECT_NAME})

# Установка C++ стандарта
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указание адресса компилятора, если он не обнаружен автоматически  
if(NOT DEFINED ENV{CC})
    set(ENV{CC} "C:/msys64/mingw64/bin/gcc.exe")
endif()

if(NOT DEFINED ENV{CXX})
    set(ENV{CXX} "C:/msys64/mingw64/bin/g++.exe")
endif()

# Установка директорий для сборки
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/archive)
# Добавляем подкаталоги, которые сами имеют CMakeLists.txt файлы

# Найти пакет OpenGL
find_package(OpenGL REQUIRED)
# Подключить SFML
find_package(SFML COMPONENTS graphics window system REQUIRED)
# Подключить GLM
find_package(glm REQUIRED)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME})
add_subdirectory(Src/Engine)
#add_subdirectory(Src/Game)


# Связать ваш проект с библиотеками
target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)
target_link_libraries(${PROJECT_NAME} glm::glm)

file(COPY ${CMAKE_SOURCE_DIR}/Src/Engine/Data DESTINATION ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME})