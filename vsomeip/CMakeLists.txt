PROJECT(SomeIpTestDemo)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++0x")

# Boost
find_package(Boost COMPONENTS system thread log)

# SOME/IP
find_package(vsomeip3)

include_directories(
	./
	Client
	Server
    ${VSOMEIP_INCLUDE_DIRS}
)

link_directories(
	${Boost_LIBRARY_DIR}
)

file(COPY ${PROJECT_SOURCE_DIR}/config/vsomeip-local.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/config/runClient.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/config/runServer.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB SomeIpTestClient_SRC
	Client/*.cpp
)

file(GLOB SomeIpTestServer_SRC
	Server/*.cpp
)

add_executable(SomeIpTestClient
	${SomeIpTestClient_SRC}
)
target_link_libraries(SomeIpTestClient vsomeip3)

add_executable(SomeIpTestServer
	${SomeIpTestServer_SRC}
)
target_link_libraries(SomeIpTestServer vsomeip3)