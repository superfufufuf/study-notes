PROJECT(multicastTest)
cmake_minimum_required(VERSION 2.8)

include_directories(
)

link_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB SRC_FILE
	*.cpp
)

add_executable(multicastTest
	${SRC_FILE}
)