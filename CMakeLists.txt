cmake_minimum_required(VERSION 3.30.0 FATAL_ERROR)
set(PROJECT_NAME EnoddGameEngine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-modules-ts")


project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES C CXX)

find_package(SDL3  REQUIRED)
find_package(Boost REQUIRED system log)





add_executable(${PROJECT_NAME} 
    Src/main.cpp

    Src/game_loop.h

    Src/Managers/manager_base.h
    Src/Managers/manager_inputs.h
    Src/Managers/manager_physics.h
    Src/Managers/manager_resourse.h
    Src/Managers/manager_scene.h
    
    Src/EngineError/engine_error.h
    Src/EngineError/engine_logging.h
    )

    target_include_directories(${PROJECT_NAME} PRIVATE  
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Managers
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/EngineError
)


target_link_libraries(${PROJECT_NAME} 
        PRIVATE 

    Boost::system
    Boost::log
    )

include(CTest)
enable_testing()
