cmake_minimum_required(VERSION 3.17)
project(MapLooper-test)

set(CMAKE_CXX_STANDARD 17)

add_executable(Test Test.cpp)
add_executable(Test-quantize Test-quantize.cpp)

include_directories(../include)
include(${PATH_TO_LINK}/AbletonLinkConfig.cmake)

find_library(LIBMAPPER mapper)
target_link_libraries(Test Ableton::Link ${LIBMAPPER})
target_link_libraries(Test-quantize Ableton::Link ${LIBMAPPER})
