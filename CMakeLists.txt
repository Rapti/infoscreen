cmake_minimum_required(VERSION 3.3)
project(Infoscreen)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set(EXECUTABLE_NAME "Infoscreen")
set(SOURCE_FILES main.cpp main.h Screen.cpp Screen.h Module.cpp Module.h ModuleTime.cpp ModuleTime.h ModuleEmpty.cpp ModuleEmpty.h ModuleSystemusage.cpp ModuleSystemusage.h ModuleRam.cpp ModuleRam.h ModuleSwp.cpp ModuleSwp.h ModuleCpu.cpp ModuleCpu.h ModuleDatausage.cpp ModuleDatausage.h ModuleInvisible.cpp ModuleInvisible.h)
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()