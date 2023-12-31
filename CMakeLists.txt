# set version
cmake_minimum_required(VERSION 3.8.2)

set(CMAKE_CXX_STANDARD 17)
 
# 重新命名工程名称，不定义就是项目名称
project(tool)

set(CMAKE_BUILD_TYPE "Debug")

set(_3rdpart_path ${CMAKE_CURRENT_SOURCE_DIR}/3rdpart)

SET(WINSYS "x11" CACHE STRING "select window system")
if (WINSYS STREQUAL "x11")
	option(D_FOR_X11_ "X11" ON)
	option(D_FOR_WAYLAND_ "WAYLAND" OFF)
elseif(WINSYS STREQUAL "wayland")
	option(D_FOR_X11_ "X11" OFF)
	option(D_FOR_WAYLAND_ "WAYLAND" ON)
else()
	option(D_FOR_X11_ "X11" ON)
	option(D_FOR_WAYLAND_ "WAYLAND" OFF)
endif()

set(extra_build_list)

list(APPEND extra_build_list "3rdpart")

option(B_BULID_VSOMEIP "build someip example" OFF)
option(B_BULID_COMMONAPI "build commonapi example" OFF)
option(B_BULID_FASTDDS "build fastdds example" OFF)
option(B_BULID_NANOMSG "build nanomsg example" OFF)
option(B_BULID_DESTRUCT "build distruct example" OFF)
option(B_BULID_NETWORK "build network example" OFF)
option(B_BULID_GBM "build network example" ON)

if(B_BULID_VSOMEIP)
    list(APPEND extra_build_list "vsomeip")
endif()
if(B_BULID_COMMONAPI)
    list(APPEND extra_build_list "commonapi")
endif()
if(B_BULID_FASTDDS)
    list(APPEND extra_build_list "fastdds")
endif()
if(B_BULID_NANOMSG)
    list(APPEND extra_build_list "nanomsg")
endif()
if(B_BULID_DESTRUCT)
    list(APPEND extra_build_list "destruct")
endif()
if(B_BULID_NETWORK)
    list(APPEND extra_build_list "network")
endif()
if(B_BULID_GBM)
    list(APPEND extra_build_list "gbm")
endif()

foreach(_target ${extra_build_list})
    message(STATUS "add build ${_target}")
    add_subdirectory(${_target})
endforeach()

unset(extra_build_list)

# add head file
include_directories(./)
include_directories(fastdds)
include_directories(future)
include_directories(rValueRef)
include_directories(shared_ptr)
include_directories(taskExecutor)
include_directories(templates)
include_directories(thread)
include_directories(window)
include_directories(destruct)
include_directories(3rdpart)
include_directories(log)
include_directories(signalSlot)
include_directories(ping)
include_directories(rand)

add_compile_options(-fno-elide-constructors) 

# copy resource to build directory
file(COPY ${PROJECT_SOURCE_DIR}/resource/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/resource/image DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# add source file
file(GLOB SRC_LIST
        "*.cpp"
        "fastdds/*.cpp"
        "future/*.cpp"
        "rValueRef/*.cpp"
        "shared_ptr/*.cpp"
        "taskExecutor/*.cpp"
        "templates/*.cpp"
        "thread/*.cpp"
        "window/*.cpp"
        "destructs/*.cpp"
        "log/*.cpp"
)


# create bin

if(D_FOR_X11_)
    message("use x11 environment")
    add_definitions(-D_FOR_X11_)
    add_executable(tool ${SRC_LIST})
    target_link_libraries(${PROJECT_NAME}
            pthread
            dl
            mylibrary
            X11
            EGL
            GLESv2
    )
elseif(D_FOR_WAYLAND_)
    message("use wayland environment")
    add_definitions(-D_FOR_WAYLAND_)
    add_executable(tool ${SRC_LIST})
    target_link_libraries(${PROJECT_NAME}
            pthread
            dl
            mylibrary
            wayland-client
            wayland-egl
            EGL
            GLESv2
    )
endif()
