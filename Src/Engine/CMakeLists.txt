

project(${PROJECT_NAME} VERSION 0.0.01 LANGUAGES C CXX)


include(CTest)
enable_testing()

add_executable(${PROJECT_NAME} 
#Src/SnakeGame.cpp
main.cpp
#engine.cpp
#engine.h
#JsonParser/json_parser.cpp
#JsonParser/json_parser.h
InputsOutputs/input_output.cpp

)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

