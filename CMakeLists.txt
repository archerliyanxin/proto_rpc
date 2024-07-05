cmake_minimum_required(VERSION 3.22)
project(proto_rpc_frame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
#add_executable(proto_rpc_frame main.cpp)

add_subdirectory(src)
add_subdirectory(proto_rpc)
add_subdirectory(network)