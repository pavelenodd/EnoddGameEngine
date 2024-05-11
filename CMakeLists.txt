cmake_minimum_required(VERSION 3.27)

project(EnoddGameEngine VERSION 0.0.01 LANGUAGES C CXX)
set(EnoddGameEngine ${PROJECT_NAME})

# Установка C++ стандарта
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CTest)
enable_testing()

# Найти пакет OpenGL
find_package(OpenGL REQUIRED)
# Подключить SFML
find_package(SFML COMPONENTS graphics window system REQUIRED)
# Подключить GLM
find_package(glm REQUIRED)


add_executable(${PROJECT_NAME} 
Src/SnakeGame.cpp
#Src/main.cpp
#Src/Engine/engine.cpp
#Src/Engine/engine.h
#Src/Engine/JsonParser/json_parser.cpp
#Src/Engine/JsonParser/json_parser.h
#Src/Engine/InputsOutputs/input_output.cpp

)

# Связать ваш проект с библиотеками
target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)
target_link_libraries(${PROJECT_NAME} glm::glm)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


#удаление папки Data в корне сборки и копирование в него из проекта 
#if(EXISTS "${CMAKE_BINARY_DIR}/Data")
 #   add_custom_target(CleanData ALL COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/Data")
#endif()

#add_custom_target(CopyData ALL COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Src/Engine/Data" "${CMAKE_BINARY_DIR}/Data"
 #   DEPENDS CleanData)

#add_dependencies(${PROJECT_NAME} CopyData)