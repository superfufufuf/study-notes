PROJECT(webTest)
cmake_minimum_required(VERSION 2.8)

include_directories(
	${_3rdpart_path}/nanomsg/include
)

link_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB SRC_FILE
	*.cpp
)

add_executable(webTest
	${SRC_FILE}
)