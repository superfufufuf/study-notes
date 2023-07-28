PROJECT(CAPITestDemo)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++0x")

# CommonAPI
include(FindPkgConfig)

# Boost
find_package(Boost COMPONENTS system thread log)

# SOME/IP
find_package(CommonAPI-SomeIP)
find_package(vsomeip3)

include_directories(
	include
	CAPITestAppA
	CAPITestAppB
    src-gen 
    ${COMMONAPI_INCLUDE_DIRS} ${COMMONAPI_SOMEIP_INCLUDE_DIRS}
    ${VSOMEIP_INCLUDE_DIRS}
)

link_directories(
    ${COMMONAPI_LIBDIR} ${COMMONAPI_DBUS_LIBDIR}
    ${COMMONAPI_SOMEIP_CMAKE_DIR}/build ${Boost_LIBRARY_DIR}
)

# message("${CMAKE_SOURCE_DIR}") #tool
# message("${PROJECT_SOURCE_DIR}") #tool/commonapi
# message("${CMAKE_CURRENT_SOURCE_DIR}") #tool/commonapi
# message("${CMAKE_BINARY_DIR}") #tool/build
# message("${CMAKE_CURRENT_BINARY_DIR}") #tool/build/commonapi
file(COPY ${PROJECT_SOURCE_DIR}/config/commonapi4someip.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/config/vsomeip-local.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/config/runningEvn DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

get_filename_component(CMAKE_CURRENT_LIST_FILE_PATH "${CMAKE_CURRENT_LIST_FILE}" PATH)
message(STATUS "CMAKE_CURRENT_LIST_FILE_PATH: ${CMAKE_CURRENT_LIST_FILE_PATH}")

if(DEFINED ENV{LD_LIBRARY_PATH})
    message("LD_LIBRARY_PATH: $ENV{LD_LIBRARY_PATH}")
endif()

#声明一个变量 wrap_BLAS_LAPACK_sources来保存wrap_BLAS_LAPACK.tar.gz 压缩包文件的名称
# set(wrap_BLAS_LAPACK_sources
# ${CMAKE_CURRENT_BINARY_DIR}/wrap_BLAS_LAPACK/CxxBLAS.hpp
# ${CMAKE_CURRENT_BINARY_DIR}/wrap_BLAS_LAPACK/CxxBLAS.cpp
# ${CMAKE_CURRENT_BINARY_DIR}/wrap_BLAS_LAPACK/CxxLAPACK.hpp
# ${CMAKE_CURRENT_BINARY_DIR}/wrap_BLAS_LAPACK/CxxLAPACK.cpp
# )

# #当目标wrap_BLAS_LAPACK_sources 被新生成时，执行COMMAND 后面的命令
# add_custom_command(
# OUTPUT
#   ${wrap_BLAS_LAPACK_sources}
# COMMAND #在构建时提取打包文件
#   ${CMAKE_COMMAND} -E tar xzf ${CMAKE_CURRENT_SOURCE_DIR}/wrap_BLAS_LAPACK.tar.gz
# COMMAND #更新提取文件的时间戳
#   ${CMAKE_COMMAND} -E touch ${wrap_BLAS_LAPACK_sources}
# WORKING_DIRECTORY #指定在何处执行命令
#   ${CMAKE_CURRENT_BINARY_DIR}
# DEPENDS #列出了自定义命令的依赖项
#   ${CMAKE_CURRENT_SOURCE_DIR}/wrap_BLAS_LAPACK.tar.gz
# COMMENT #在构建时打印状态消息
#   "Unpacking C++ wrappers for BLAS/LAPACK"
# VERBATIM #告诉CMake为生成器和平台生成正确的命令，从而确保完全独立。
# )

file(GLOB CAPITestA_SRC
	src/CAPITestAppA/*.cpp
)

file(GLOB CAPITestB_SRC
	src/CAPITestAppB/*.cpp
)

add_executable(CAPITestA
	${CAPITestA_SRC}
	src-gen/v1/commonapi/examplesA/CAPITestASomeIPStubAdapter.cpp
	src-gen/v1/commonapi/examplesA/CAPITestASomeIPDeployment.cpp
	src-gen/v1/commonapi/examplesB/CAPITestBSomeIPProxy.cpp
	src-gen/v1/commonapi/examplesB/CAPITestBSomeIPDeployment.cpp
)
target_link_libraries(CAPITestA CommonAPI CommonAPI-SomeIP vsomeip3)

add_executable(CAPITestB
	${CAPITestB_SRC}
	src-gen/v1/commonapi/examplesA/CAPITestASomeIPProxy.cpp
	src-gen/v1/commonapi/examplesA/CAPITestASomeIPDeployment.cpp
	src-gen/v1/commonapi/examplesB/CAPITestBSomeIPStubAdapter.cpp
	src-gen/v1/commonapi/examplesB/CAPITestBSomeIPDeployment.cpp
)
target_link_libraries(CAPITestB CommonAPI CommonAPI-SomeIP vsomeip3)