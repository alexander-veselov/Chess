cmake_minimum_required(VERSION 3.30)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(Chess LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(imgui CONFIG REQUIRED)

set(INCLUDE_FOLDER "include")
set(SOURCE_FOLDER "src")
file(GLOB HEADERS ${INCLUDE_FOLDER}/*.h)
file(GLOB SOURCES ${SOURCE_FOLDER}/*.cpp)
add_executable(Chess ${HEADERS} ${SOURCES})
target_include_directories(Chess PUBLIC ${INCLUDE_FOLDER})
target_link_libraries(Chess PRIVATE imgui::imgui)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Chess)