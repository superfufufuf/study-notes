PROJECT(nettest)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

include_directories(./)

add_executable(multicast multicast.cpp)
target_link_libraries(multicast pthread)

add_executable(udp_server udp_server.cpp)

add_executable(udp_client udp_client.cpp)