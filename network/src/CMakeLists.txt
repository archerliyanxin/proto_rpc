include(CheckFunctionExists)
check_function_exists(accept4 HAVE_ACCEPT)
if(NOT HAVE_ACCEPT)
    set_source_files_properties(SocketsOps.cpp PROPERTIES COMPILE_FLAGS "-DNO_ACCEPT4")
endif ()
set(net_SRCS
        SocketsOps.cpp
        InetAddress.cpp
        Socket.cpp Channel.cpp TcpServer.cpp EventLoop.cpp Poller.cpp defaultPoller.cpp EpollPoller.cpp EventLoopThread.cpp EventLoopThreadPool.cpp
        Acceptor.cpp
        Buffer.cpp
        TcpConncetion.cpp
        Connector.cpp
        TcpClient.cpp
        utils.cpp)
add_library(network ${net_SRCS})
target_include_directories(network PUBLIC ../include)
install(TARGETS network DESTINATION ${PROJECT_BINARY_DIR}/lib)