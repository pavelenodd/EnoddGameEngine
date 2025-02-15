cmake_minimum_required(VERSION 3.28.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_NAME EnoddGameEngine)

project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES C CXX)

find_library(SFML sfml-graphics sfml-window sfml-system)
find_library(Boost boost_system boost_log)





add_executable(${PROJECT_NAME} 
    Src/main.cpp


    Src/game_loop.h
    #менеджеры 
    Src/Managers/manager_base.h
    Src/Managers/manager_inputs.h
    Src/Managers/manager_physics.h
    Src/Managers/manager_resourse.h
    Src/Managers/manager_scene.h
    
    Src/EngineError/engine_error.h
    Src/EngineError/engine_logging.h
    )

target_link_libraries(${PROJECT_NAME} 
        PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system
    boost_system
    boost_log
    )

include(CTest)
enable_testing()
