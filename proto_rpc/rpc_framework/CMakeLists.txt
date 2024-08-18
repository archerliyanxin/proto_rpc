set(SOURCES
        rpc.pb.cc
        rpcServer.cpp
        rpcCodec.cpp
        rpcChannel.cpp
        rpcServer.cpp)
add_library(rpc_server ${SOURCES})
message("../..")
message("${PROTOBUF_INCLUDE_DIRS}" )
target_include_directories(rpc_server PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR} /
        ${PROJECT_SOURCE_DIR})

target_link_libraries(rpc_server PUBLIC protobuf rpc_proto)
install(TARGETS  rpc_server DESTINATION  ${PROJECT_BINARY_DIR}/lib)
