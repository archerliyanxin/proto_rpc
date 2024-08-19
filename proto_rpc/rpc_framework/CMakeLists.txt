set(SOURCES
        rpc.pb.cc
        rpcServer.cpp
        rpcCodec.cpp
        rpcChannel.cpp
)
add_library(rpc_framework ${SOURCES})
message("../..")
message("${PROTOBUF_INCLUDE_DIRS}" )
target_include_directories(rpc_framework PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR} /
        ${PROJECT_SOURCE_DIR})

target_link_libraries(rpc_framework PUBLIC protobuf rpc_proto z network)
install(TARGETS  rpc_framework DESTINATION  ${PROJECT_BINARY_DIR}/lib)
