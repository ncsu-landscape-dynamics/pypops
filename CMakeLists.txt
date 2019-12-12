# Works with X as minimum and tested through Y (X...Y)
cmake_minimum_required(VERSION 3.11...3.13)

project(PyPoPS
    VERSION 0.9
    DESCRIPTION "Python interface to PoPS (Pest or Pathogen Spread) Model C++ library"
    LANGUAGES CXX)

#add_library(PoPS fun.cpp)
#include_directories("${PROJECT_SOURCE_DIR}/")

add_subdirectory(pybind11)
add_subdirectory(PoPS)

pybind11_add_module(pypops src/pypops.cpp)

target_link_libraries(pypops PRIVATE pops)

# Testing only available if this is the main app
#if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) AND BUILD_TESTING)
    #add_subdirectory(tests)
#endif()

# Enables testing for subdirectories and adds target test
include(CTest)

add_subdirectory(tests)
