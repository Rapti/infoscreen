cmake_minimum_required(VERSION 3.3)
project(Infoscreen)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set(EXECUTABLE_NAME "Infoscreen")
set(SOURCE_FILES src/main.cpp src/main.h src/Screen.cpp src/Screen.h src/Module.cpp src/Module.h src/ModuleTime.cpp src/ModuleTime.h src/ModuleEmpty.cpp src/ModuleEmpty.h src/ModuleSystemusage.cpp src/ModuleSystemusage.h src/ModuleRam.cpp src/ModuleRam.h src/ModuleSwp.cpp src/ModuleSwp.h src/ModuleCpu.cpp src/ModuleCpu.h src/ModuleDatausage.cpp src/ModuleDatausage.h src/ModuleInvisible.cpp src/ModuleInvisible.h src/Grid.cpp src/Grid.h src/ModulePing.cpp src/ModulePing.h)
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()