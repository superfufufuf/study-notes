PROJECT(gbmTest)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

include_directories(./)
include_directories(drm)

add_executable(drm_test drmTest.cpp)
target_link_libraries(drm_test drm)