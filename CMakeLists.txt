# Works with X as minimum and tested through Y (X...Y)
cmake_minimum_required(VERSION 3.11...3.13)

project(PyPoPS
    VERSION 0.9
    DESCRIPTION "Python interface to PoPS (Pest or Pathogen Spread) Model C++ library"
    LANGUAGES CXX)

#add_library(PoPS fun.cpp)
#include_directories("${PROJECT_SOURCE_DIR}/")

add_subdirectory(pybind11)
add_subdirectory(pops-core)

pybind11_add_module(_pypops src/pypops.cpp include/raster.hpp include/helpers.hpp)

target_include_directories(_pypops PRIVATE include)

target_link_libraries(_pypops PRIVATE pops)

if(MSVC)
  target_compile_options(_pypops PRIVATE /W4 /WX)
else()
  target_compile_options(_pypops PRIVATE -Wall -Wextra -pedantic -Werror)
endif()


# Testing only available if this is the main app
#if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) AND BUILD_TESTING)
    #add_subdirectory(tests)
#endif()

# Enables testing for subdirectories and adds target test
include(CTest)

add_subdirectory(tests)
