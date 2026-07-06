cmake_minimum_required(VERSION 3.30)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(Chess LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(imgui CONFIG REQUIRED)
find_package(lunasvg CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Core library
set(CORE_INCLUDE_FOLDER "include/core")
set(CORE_SOURCE_FOLDER "src/core")
file(GLOB CORE_HEADERS ${CORE_INCLUDE_FOLDER}/chess/core/*.h)
file(GLOB CORE_SOURCES ${CORE_SOURCE_FOLDER}/*.cpp)
add_library(Core STATIC ${CORE_HEADERS} ${CORE_SOURCES})
target_include_directories(Core PUBLIC ${CORE_INCLUDE_FOLDER})

# Engine library
set(ENGINE_INCLUDE_FOLDER "include/engine")
set(ENGINE_SOURCE_FOLDER "src/engine")
file(GLOB ENGINE_HEADERS ${ENGINE_INCLUDE_FOLDER}/chess/engine/*.h)
file(GLOB ENGINE_SOURCES ${ENGINE_SOURCE_FOLDER}/*.cpp)
add_library(Engine STATIC ${ENGINE_HEADERS} ${ENGINE_SOURCES})
target_include_directories(Engine PUBLIC ${ENGINE_INCLUDE_FOLDER})

# Application executable
set(APPLICATION_INCLUDE_FOLDER "include/application")
set(APPLICATION_SOURCE_FOLDER "src/application")
file(GLOB APPLICATION_HEADERS ${APPLICATION_INCLUDE_FOLDER}/chess/application/*.h)
file(GLOB APPLICATION_SOURCES ${APPLICATION_SOURCE_FOLDER}/*.cpp)
add_executable(Application ${APPLICATION_HEADERS} ${APPLICATION_SOURCES})
target_include_directories(Application PUBLIC ${APPLICATION_INCLUDE_FOLDER})
target_link_libraries(Application PRIVATE imgui::imgui lunasvg::lunasvg opengl32 Core)
set_property(TARGET Application PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Test executable
set(TEST_INCLUDE_FOLDER "include/test")
set(TEST_SOURCE_FOLDER "src/test")
file(GLOB TEST_HEADERS ${TEST_INCLUDE_FOLDER}/chess/test/*.h)
file(GLOB TEST_SOURCES ${TEST_SOURCE_FOLDER}/*.cpp)
add_executable(Test ${TEST_HEADERS} ${TEST_SOURCES})
target_include_directories(Test PUBLIC ${TEST_INCLUDE_FOLDER})
target_link_libraries(Test PRIVATE Core GTest::gtest GTest::gtest_main)
set_property(TARGET Test PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Application)

add_custom_command(TARGET Application POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:Application>/assets
)