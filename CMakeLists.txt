cmake_minimum_required(VERSION 3.30)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(Chess LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(imgui CONFIG REQUIRED)
find_package(lunasvg CONFIG REQUIRED)

# Engine library
set(ENGINE_INCLUDE_FOLDER "include/engine")
set(ENGINE_SOURCE_FOLDER "src/engine")
file(GLOB ENGINE_HEADERS ${ENGINE_INCLUDE_FOLDER}/chess/engine/*.h)
file(GLOB ENGINE_SOURCES ${ENGINE_SOURCE_FOLDER}/*.cpp)
add_library(Engine STATIC ${ENGINE_HEADERS} ${ENGINE_SOURCES})
target_include_directories(Engine PUBLIC ${ENGINE_INCLUDE_FOLDER})

# Game library
set(GAME_INCLUDE_FOLDER "include/game")
set(GAME_SOURCE_FOLDER "src/game")
file(GLOB GAME_HEADERS ${GAME_INCLUDE_FOLDER}/chess/game/*.h)
file(GLOB GAME_SOURCES ${GAME_SOURCE_FOLDER}/*.cpp)
add_library(Game STATIC ${GAME_HEADERS} ${GAME_SOURCES})
target_include_directories(Game PUBLIC ${GAME_INCLUDE_FOLDER})

# Application executable
set(APPLICATION_INCLUDE_FOLDER "include/application")
set(APPLICATION_SOURCE_FOLDER "src/application")
file(GLOB APPLICATION_HEADERS ${APPLICATION_INCLUDE_FOLDER}/chess/application/*.h)
file(GLOB APPLICATION_SOURCES ${APPLICATION_SOURCE_FOLDER}/*.cpp)
add_executable(Application ${APPLICATION_HEADERS} ${APPLICATION_SOURCES})
target_include_directories(Application PUBLIC ${APPLICATION_INCLUDE_FOLDER})
target_link_libraries(Application PRIVATE imgui::imgui lunasvg::lunasvg opengl32)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Application)