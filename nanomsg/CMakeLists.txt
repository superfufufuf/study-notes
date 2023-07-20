PROJECT(nanomsgTestDemo)
cmake_minimum_required(VERSION 2.8)

include_directories(
	${_3rdpart_path}/nanomsg/include
)

link_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB nanomsgTest_SRC
	*.cpp
)

add_executable(nanomsgTest
	${nanomsgTest_SRC}
)
target_link_libraries(nanomsgTest nanomsg)