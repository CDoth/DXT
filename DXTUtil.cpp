#include "DXTUtil.h"


using namespace DXTLogsNamespace;

DXTUtil::DXTAddress::DXTAddress() { clear(); }
DXTUtil::DXTAddress::DXTAddress(int port, const char *address) { setStats(__address, port, address);}
DXTUtil::DXTAddress::DXTAddress(const sockaddr_in &a) : __address(a) {}

bool DXTUtil::DXTAddress::set(int port, const char *address) { return DXTUtil::setStats(__address, port, address); }
void DXTUtil::DXTAddress::clear() {
    zero_mem(&__address, 1);
    __address.sin_port = htons(-1);
}

bool DXTUtil::DXTAddress::isValid() const { return port() > -1; }
int DXTUtil::DXTAddress::port() const { return ntohs(__address.sin_port); }
const char *DXTUtil::DXTAddress::address() const { return inet_ntoa(__address.sin_addr); }


DXTUtil::DXTAddress DXTUtil::extractLocalAddress(SOCKET socket) {

    if( invalid_socket(socket) ) return DXTAddress();

    sockaddr_in __addr;
    int len = sizeof(sockaddr);
    if( getsockname(socket, (sockaddr*)&__addr, &len) < 0 ) {
        DL_ERROR(1, "getsockname");
        return DXTAddress();
    }
    return DXTAddress(__addr);
}
DXTUtil::DXTAddress DXTUtil::extractPeerAddress(SOCKET socket) {
    if( invalid_socket(socket) ) return DXTAddress();

    sockaddr_in __addr;
    int len = sizeof(sockaddr);
    if( getpeername(socket, (sockaddr*)&__addr, &len) < 0 ) {
        DL_ERROR(1, "getpeername");
        return DXTAddress();
    }
    return DXTAddress(__addr);
}

int DXTUtil::port(const sockaddr_in &a) { return ntohs(a.sin_port); }
const char *DXTUtil::address(const sockaddr_in &a) { return inet_ntoa(a.sin_addr); }
SOCKET DXTUtil::socket(int af, int type, int protocol) {

    SOCKET new_socket = INVALID_SOCKET;
    if( (new_socket = ::socket(af, type, protocol) ) == INVALID_SOCKET) {
        DXT_ERROR;
        return INVALID_SOCKET;
    }
    return new_socket;
}
SOCKET DXTUtil::accept(SOCKET s, sockaddr *addr, int *addrlen) {

    if( invalid_socket(s) ) {
        DL_BADVALUE(1, "socket");
        return INVALID_SOCKET;
    }
    SOCKET accept_socket = INVALID_SOCKET;

    DL_INFO(1, "Try accept...");
    #ifdef _WIN32
    if( (accept_socket = ::accept(s, addr, addrlen) ) == INVALID_SOCKET) {
        DXT_ERROR;
        return INVALID_SOCKET;
    }
    #else
    if( (accept_socket = ::accept(s, addr, (socklen_t *)addrlen)) == INVALID_SOCKET) {
        DXT_ERROR;
        return INVALID_SOCKET;
    }
    #endif
    DL_INFO(1, "Accepted");

    return accept_socket;
}
bool DXTUtil::setStats(sockaddr_in &in, int port, const char *address) {

    if( DXT_CHECK_PORT(port) == false ) {
        DL_BADVALUE(1, "port [%d]", port);
        return false;
    }

    memset(&in, 0, sizeof(sockaddr_in));
    in.sin_port = htons(port);
    in.sin_family = AF_INET;

    #ifdef _WIN32
    in.sin_addr.S_un.S_addr = address ? inet_addr(address) : htonl(INADDR_ANY);
    #else
    in.sin_addr.s_addr = address ? inet_addr(address) : htonl(INADDR_ANY);
    #endif

    return true;
}
bool DXTUtil::is_valid_socket(SOCKET s) {

    if( s && s != INVALID_SOCKET ) return true;
    return false;
}
bool DXTUtil::invalid_socket(SOCKET s) {
    return !is_valid_socket(s);
}

bool DXTUtil::closesocket(SOCKET s) {

    if( invalid_socket(s) ) {
        DL_BADVALUE(1, "socket");
        return false;
    }
#ifdef _WIN32
    if( ::closesocket(s) == SOCKET_ERROR ) {
        DXT_ERROR;
        return false;
    }
#else
    if( close(s) < 0 ) {
        DXT_ERROR;
        return false;
    }
#endif

    return true;
}
bool DXTUtil::bind(SOCKET s, const sockaddr *addr) {

    if( invalid_socket(s) ) {
        DL_BADVALUE(1, "socket");
        return false;
    }
    if(::bind(s, addr, sockaddr_in__len) == SOCKET_ERROR) {
        DXT_ERROR;
        return false;
    }
    return true;
}
bool DXTUtil::listen(SOCKET s, int backlog) {

    if( invalid_socket(s) ) {
        DL_BADVALUE(1, "socket");
        return false;
    }
    if(::listen(s, backlog) == SOCKET_ERROR) {
        DXT_ERROR;
        return false;
    }
    return true;
}
bool DXTUtil::connect(SOCKET s, const sockaddr *name, int namelen) {

    if( invalid_socket(s) ) {
        return false;
    }

        if( ::connect(s, name, namelen) == SOCKET_ERROR) {
//            int e = DXT_LAST_ERROR;
//            if(e != DXT_WOULD_BLOCK) {
//                if(e == DXT_EISCONN) {
//                    DL_INFO(1, "Already connected");
//                    return true;
//                } else {
                    DXT_ERROR;
                    return false;
//                }
//            }
        }



    return true;
}
bool DXTUtil::shutdown(SOCKET s, int how) {

    if( invalid_socket(s) ) {
//        DL_BADVALUE(1, "socket");
        return false;
    }
    if( ::shutdown(s, how) == SOCKET_ERROR) {
        DXT_ERROR;
        return false;
    }
    return true;
}
int DXTUtil::select(SOCKET socket, fd_set *read_set, fd_set *write_set, fd_set *err_set, timeval *tv) {

    int r;
#ifdef _WIN32
    r = select(0, read_set, write_set, err_set, tv);
#else
    r = select(socket+1, read_set, write_set, err_set, tv);
#endif
    if(r == SOCKET_ERROR) {
        DXT_ERROR;
        return SOCKET_ERROR;
    }
    return r;
}






