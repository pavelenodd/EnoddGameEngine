

project(${PROJECT_NAME} VERSION 0.0.01 LANGUAGES C CXX)


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
#Src/SnakeGame.cpp
main.cpp
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







