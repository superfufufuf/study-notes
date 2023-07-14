PROJECT(FDDSTestDemo)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++0x")

project(HelloWorldExample)

# Find requirements
include(FindPkgConfig)
find_package(fastcdr REQUIRED)
find_package(foonathan_memory REQUIRED)
find_package(fastrtps REQUIRED)

include_directories(
	FDDSTestAppA
	FDDSTestAppB
    src-gen
)

file(GLOB FDDS_GEN_SRC
	src-gen/*.cxx
)

file(GLOB FDDSTestA_SRC
	src/FDDSTestAppA/*.cpp
)

file(GLOB FDDSTestB_SRC
	src/FDDSTestAppB/*.cpp
)

add_executable(FDDSTestA
	${FDDS_GEN_SRC}
	${FDDSTestA_SRC}
)
target_link_libraries(FDDSTestA fastrtps fastcdr)

add_executable(FDDSTestB
	${FDDS_GEN_SRC}
	${FDDSTestB_SRC}
)
target_link_libraries(FDDSTestB fastrtps fastcdr)