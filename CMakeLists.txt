cmake_minimum_required(VERSION 3.0.0)
project(last_stage_mod VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()


string(TOLOWER ${CMAKE_SYSTEM_NAME} PlatformDir)
option(MAKE_EXAMPLE "\
Compile a small example application that demonstrates ProgramModification \
class usage alongside the main application" OFF)
option(SUPERHEX_INVULN "Invulnerability for testing purposes" OFF)

if (SUPERHEX_INVULN)
   add_definitions(-DSUPERHEX_INVULN=1)
endif()

if(MSVC)
   add_compile_options(/W2 /WX)
else()
   add_compile_options(-Wall -Wextra -Werror)
endif()

include_directories(${PlatformDir} common)
file(GLOB StageModSources ${PlatformDir}/*.cpp common/*.cpp)
add_executable(last_stage_mod ${StageModSources})
set_property(TARGET last_stage_mod PROPERTY CXX_STANDARD 17)

if(MAKE_EXAMPLE)
    file(GLOB ExampleSources example/main.cpp common/*.cpp)
    add_executable(example ${ExampleSources})
    set_property(TARGET example PROPERTY CXX_STANDARD 17)
endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
