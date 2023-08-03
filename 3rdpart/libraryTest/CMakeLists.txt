PROJECT(libraryTest)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++0x")

include_directories(
	./
)

file(GLOB SRC_FILES
	*.cpp
)

add_library(mylibrary SHARED
	${SRC_FILES}
)