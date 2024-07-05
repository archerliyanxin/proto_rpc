find_package(Protobuf REQUIRED)
set(PROTO_FILES rpc.proto)
add_library(rpc_proto ${PROTO_FILES})
target_link_libraries(rpc_proto PUBLIC
        protobuf::libprotobuf)
target_include_directories(rpc_proto PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROTOBUF_INCLUDE_DIRS})
message("ss=${PROTOBUF_INCLUDE_DIRS}")
protobuf_generate(TARGET rpc_proto LANGUAGE cpp)
install(TARGETS rpc_proto DESTINATION ${PROJECT_BINARY_DIR}/lib)
