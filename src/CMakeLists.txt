find_package(Protobuf  REQUIRED)
#find_package(Glog REQUIRED)
set(PROTO_FILES message.proto)
add_library(monitor_proto ${PROTO_FILES})
target_link_libraries(monitor_proto
        PUBLIC
            protobuf::libprotobuf)

#
target_include_directories(monitor_proto PUBLIC
        ${PROTOBUF_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate(TARGET monitor_proto LANGUAGE cpp)
message("kk=${PROJECT_BINARY_DIR}")
install(TARGETS monitor_proto
        DESTINATION ${PROJECT_BINARY_DIR}/lib)
#message(${PROJECT_BINARY_DIR})
add_executable(proto_server server.cc)
#target_compile_definitions(proto_server PUBLIC HAVE_LIB_GFLAGS HAVE_GLOG)
target_link_libraries(proto_server monitor_proto rpc_framework network)
target_include_directories(proto_server PUBLIC
        ${PROJECT_SOURCE_DIR}/proto_rpc
        )
add_executable(proto_client client.cpp)
target_link_libraries(proto_client PUBLIC
        monitor_proto rpc_framework network)
target_include_directories(proto_client PUBLIC
        ${PROJECT_SOURCE_DIR}/proto_rpc)
install(TARGETS proto_server proto_client
        DESTINATION ${PROJECT_BINARY_DIR}/bin)
