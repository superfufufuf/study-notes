PROJECT(gbmTest)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

include_directories(./)

add_library(DisPlayServer SHARED DisplayServer.cpp)
target_link_libraries(DisPlayServer gbm png EGL GL)

add_executable(drm_test drmTest.cpp)
target_link_libraries(drm_test drm)