#include "DXTServer.h"

using namespace DXTLogsNamespace;
DXTServer::DXTServer() {

}
DXTServer::~DXTServer() {

}
bool DXTServer::isOpen(int port) { return socket_map.find(port) != socket_map.end(); }
bool DXTServer::create(int port) {

    if( DXT_CHECK_PORT(port) == false ) {
        DL_BADVALUE(1, "port: [%d]", port);
        return false;
    }
    if( socket_map.find(port) != socket_map.end() ) {
        DL_ERROR(1, "No bound socket with port [%d]", port);
        return false;
    }
    int type = SOCK_STREAM;
    int protocol = 0;
    SOCKET new_socket = DXTUtil::socket(AF_INET, type, protocol);
    if(new_socket == INVALID_SOCKET) {
        DL_FUNCFAIL(1, "socket");
        return false;
    }
    struct sockaddr_in addr;
    DXTUtil::setStats(addr, port);

    #ifndef WIN32
    int reuse_value = 1;
    if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_value, sizeof(int)) < 0) {
        DL_FUNCFAIL(1, "setsockopt");
        return false;
    }
    #endif

    if(  DXTUtil::bind(new_socket, (sockaddr*)&addr) == false  ) {
        DL_FUNCFAIL(1, "bind");
        return false;
    }
    if(  DXTUtil::listen(new_socket, 10) == false  ) {
        DL_FUNCFAIL(1, "listen");
        return false;
    }
    boundSocket bs;
    bs.socket = new_socket;
//    bs.localAddress = addr;
    socket_map[port] = bs;

    return true;
}
bool DXTServer::acceptIncoming(int port, int block) {

    auto i = socket_map.find(port);
    if(i == socket_map.end()) {
        DL_ERROR(1, "No bound port [%d]", port);
        return false;
    }
    boundSocket bs = i->second;
    block = block > 0 ? block : 0;

    if( block || checkIncomingConnection(port) ) {

        sockaddr_in __addr;
        int __addr_len = sizeof(sockaddr);
        SOCKET connection_socket = INVALID_SOCKET;
        connection_socket = DXTUtil::accept(bs.socket, (sockaddr*)&__addr, &__addr_len);


        if( DXTUtil::invalid_socket(connection_socket) ) {
            DL_FUNCFAIL(1, "accept");
            return false;
        }

//        DL_INFO(1, "DXTServer accepted socket: [%d] listen socket: [%d]", connection_socket, bs.socket);

        boundSocket __accepted__;
        __accepted__.socket = connection_socket;
        acceptedQueue.append(__accepted__);
        return true;
    }

    return false;
}
bool DXTServer::checkIncomingConnection(int port) const  {

    auto i = socket_map.find(port);
    if(i == socket_map.end()) {
        DL_ERROR(1, "No bound port [%d]", port);
        return false;
    }
    boundSocket bs = i->second;

    SOCKET socket = bs.socket;
    if( socket == INVALID_SOCKET ) {
        DL_BADVALUE(1, "socket (port: [%d])", port);
        return false;
    }

    fd_set fdread;
    FD_ZERO(&fdread);
    FD_SET(socket, &fdread);
    int select_read = 0;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    if( (select_read = DXTUtil::select(socket, &fdread, nullptr, nullptr, &timeout)) < 0 ) {
        DXT_ERROR;
        return false;
    }

    for(int i=0;i < select_read; ++i) {
        if(FD_ISSET(socket, &fdread)) {
            return true;
        }
    }

    return false;
}
bool DXTServer::getAccepted(boundSocket &inLastAccepted) {

    if( acceptedQueue.empty() ) {
        DL_ERROR(1, "No accepted connections");
        return false;
    }
    inLastAccepted = acceptedQueue.back();
    acceptedQueue.pop_back();
    return true;

}

