cmake_minimum_required(VERSION 3.27)

# Название проекта
project(MyProject ${PROJECT_NAME})

# Установка директорий для сборки
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/archive)
# Добавляем подкаталоги, которые сами имеют CMakeLists.txt файлы

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME})
add_subdirectory(Src/Engine)
#add_subdirectory(Game)




file(COPY ${CMAKE_SOURCE_DIR}/Src/Engine/Data DESTINATION ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME})