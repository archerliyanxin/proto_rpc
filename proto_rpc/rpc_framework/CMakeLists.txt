set(SOURCES
        rpc_server1.cpp
        rpc.pb.cc
        rpcServer.h
        rpcServer.cpp)
add_library(rpc_server ${SOURCES})
message("../..")
target_include_directories(rpc_server PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR} /
        ${PROTOBUF_INCLUDE_DIRS} /
        ${PROJECT_SOURCE_DIR})

target_link_libraries(rpc_server PUBLIC protobuf glog rpc_proto)
install(TARGETS  rpc_server DESTINATION  ${PROJECT_BINARY_DIR}/lib)
