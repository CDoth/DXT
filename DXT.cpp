#include "DXT.h"
#include <DProfiler.h>

using namespace DXTLogsNamespace;
DXTServer dxt_global_server;

DXT::DXT() : __type(TCP) {
    __clear();
}
DXT::DXT(DXT::Type t) : __type(t) {
    __clear();
}
DXT::~DXT() {
    if( __auto_dc ) stop();
}
bool DXT::makeServer(int port) {

    switch (__type) {

    case TCP:

        if( dxt_global_server.isOpen(port) ) return true;
        return dxt_global_server.create(port);
        break;

    case UDP:
        return bind(port);
        break;

    }

    return false;
}
bool DXT::bind(int port) {

    if( __state != Empty ) {
        DL_ERROR(1, "Socket in [%s] state, stop it", stateName_c());
        return false;
    }
    if( DXTUtil::invalid_socket(__socket) ) {
        if( __createSocket() == false ) {
            DL_ERROR(1, "Can't create socket");
            return false;
        }
    }
    if( __boundAddress.set(port) == false ) {
        DL_FUNCFAIL(1, "set");
        __clear();
        return false;
    }
    if(  DXTUtil::bind(__socket, __boundAddress) == false  ) {
        DL_FUNCFAIL(1, "bind");
        if( DXT_LAST_ERROR == DXT_EADDRINUSE ) {

        }
        __clear();
        return false;
    }
    __state = Bound;
    return true;
}
bool DXT::accept(int port) {

    if( __type == UDP ) {
        DL_WARNING(1, "accept for TCP type");
        return false;
    }
    if( __state != Empty ) {
        DL_ERROR(1, "Socket in [%s] state, stop it", stateName_c());
        return false;
    }
    if( !DXT_CHECK_PORT(port) ) {
        DL_BADVALUE(1, "port");
        return false;
    }

//    DL_INFO(1, "DXT:: try accept...");
    if( dxt_global_server.acceptIncoming(port, 0) ) {
        boundSocket s;
        if( dxt_global_server.getAccepted(s)  == false ) {
            DL_FUNCFAIL(1, "getAccepted");
            return false;
        }

        __socket = s.socket;
        __state = Accepted;
        __boundAddress.set(port);
        __remoteAddress = DXTUtil::extractPeerAddress(__socket);
        if( !__remoteAddress.isValid() ) {
            DL_WARNING(1, "Can't extract peer address");
        }
//        DL_INFO(1, "DXT:: accepted socket: [%d]", __socket);
        return true;
    }
    return false;
}
bool DXT::connect(int port, const char *address) {

    if( __state == Empty ) {
        if( DXTUtil::invalid_socket(__socket) ) {
            if( __createSocket() == false ) {
                DL_ERROR(1, "Can't create socket");
                return false;
            }
        }
    } else if ( __state == Bound ) {
        if( DXTUtil::invalid_socket(__socket) ) {
            DL_BADVALUE(1, "Socket");
            return false;
        }
    } else if ( __type != UDP ) {
        DL_WARNING(1, "TCP socket in [%s] state", stateName_c());
        return false;
    }

    sockaddr_in peerAddress;
    if( DXTUtil::setStats(peerAddress, port, address) == false ) {
        DL_FUNCFAIL(1, "setStats");
        __clear();
        return false;
    }
    DL_INFO(1, "Try connect (socket: [%d])...", __socket);

    if( DXTUtil::connect( __socket, (sockaddr* )&peerAddress, sizeof(sockaddr_in)) == false ) {
        DL_FUNCFAIL(1, "connect");
        __clear();
        return false;
    }
    if( __state == Empty )  {
        __boundAddress = DXTUtil::extractLocalAddress(__socket);
    }
    __remoteAddress.set(port, address);

    __state = Connected;
    DL_INFO(1, "connected");
    return true;
}
bool DXT::newConnection(int port) {
    if( __type == UDP ) {
        DL_WARNING(1, "There is UDP state");
        return false;
    }
    return dxt_global_server.checkIncomingConnection(port);
}
bool DXT::changeType(DXT::Type t) {

    if( __state != Empty ) {
        DL_ERROR(1, "Socket in [%s] state, stop it", stateName_c());
        return false;
    }
    __type = t;
    return true;
}
bool DXT::stop() {

    DXTUtil::shutdown(__socket, SD_BOTH);
    DXTUtil::closesocket(__socket);
    __socket = INVALID_SOCKET;
    __state = Empty;
    __clear();

    return true;
}
bool DXT::readable(int sec, int usec) {

    if( DXTUtil::invalid_socket(__socket) ) {
        DL_BADVALUE(1, "Socket");
        return false;
    }
    fd_set fdread;
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;

    int select_read = 0;
    FD_ZERO(&fdread);
    FD_SET(__socket, &fdread);

#ifdef _WIN32
    select_read = DXTUtil::select( __socket, &fdread, nullptr, nullptr, &timeout );
#else
    select_read = __select(_socket, &fdread, nullptr, nullptr, &timeout);
#endif
    return FD_ISSET(__socket, &fdread);
}
bool DXT::writable(int sec, int usec) {

    if( DXTUtil::invalid_socket(__socket) ) {
        DL_BADVALUE(1, "Socket");
        return false;
    }

    fd_set fdwrite;
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;

    FD_ZERO(&fdwrite);
    FD_SET(__socket, &fdwrite);
    int select_write = 0;

#ifdef _WIN32
    select_write = DXTUtil::select( __socket, nullptr, &fdwrite, nullptr, &timeout );
#else
    select_write = __select(_socket, nullptr, &fdwrite, nullptr, &timeout);
#endif

    return FD_ISSET(__socket, &fdwrite);
}
int DXT::send(const void *data, int size, int flag) {
    if( __type == TCP ) return __tcp_send(data, size, flag);
    return __udp_send(data, size, flag);
}
int DXT::receive(void *buffer, int size, int flag) {
    if( __type == TCP ) return __tcp_receive(buffer, size, flag);
    return __udp_receive(buffer, size, flag);
}
int DXT::localPort() const { return __extractLocalPort(); }
int DXT::peerPort() const { return __extractPeerPort(); }

std::string DXT::stateName() const {
    return stateName_c();
}
const char *DXT::stateName_c() const {
    switch (__state) {
        case Empty: return "Empty";
        case Bound: return "Bound";
        case Accepted: return "Accepted";
        case Connected: return "Connected";
    }
    return nullptr;
}
DXT::Type DXT::type() const {
    return __type;
}
DXT::InnerState DXT::state() const {
    return __state;
}
bool DXT::isBound() const {
    return __state == Bound || __state == Connected;
}
bool DXT::isValid() const {
    return DXTUtil::is_valid_socket(__socket);
}
int DXT::errorCode() const {
    return DXT_LAST_ERROR;
}
int DXT::__tcp_send(const void *data, int size, int flag) {

    if( data == nullptr || size < 0 ) return -1;
    if( size == 0 ) return 0;

    const char *d = reinterpret_cast<const char*>(data);
    int s = size;
    if( data == lastSendBuffer ) {
        d += total_sb;
        s -= total_sb;
    }
    if(  (sb = ::send(__socket, (const char*)data, s, flag)) < 0) {
        DXT_ERROR;
        DL_FUNCFAIL(1, "send: [%d] socket: [%d]", sb, __socket);
        return sb;
    }
    if( sb == size ) {
        lastSendBuffer = nullptr;
        total_sb = 0;
        return size;
    } else {
        lastSendBuffer = data;
        total_sb += rb;
    }

    return total_sb;
}
int DXT::__udp_send(const void *data, int size, int flag) {

    if( size > maxDatagramSize ) {
        DL_BADVALUE(1, "Too long data size [%d] (chack maximum datagram size)", size);
        return -1;
    }
    const char *d = reinterpret_cast<const char*>(data);
    if( __state == Connected ) {
        if(  (sb = ::send(__socket, (const char*)data, size, flag)) < 0) {
            DL_FUNCFAIL(1, "send: [%d]", sb);
            return sb;
        }
    } else {

        if( (sb = ::sendto(__socket, d, size, flag, __udpTarget, sizeof(sockaddr_in))) < 0 ) {
            DL_ERROR(1, "Send fail");
            return -1;
        }
    }


    return sb;
}
int DXT::__tcp_receive(void *place, int bytes, int flag) {

    char *d = reinterpret_cast<char*>(place);
    int s = bytes;

    if( place == lastReceiveBuffer ) {
        d += total_rb;
        s -= total_rb;
    }
    if( (rb = recv(__socket, d, s, flag)) < 0 ) {
        DL_ERROR(1, "recv fault");
        return rb;
    }
    if( rb == bytes ) {
        lastReceiveBuffer = nullptr;
        total_rb = 0;
        return bytes;
    } else {
        lastReceiveBuffer = place;
        total_rb += rb;
    }

    return total_rb;
}
int DXT::__udp_receive(void *buffer, int bufferSize, int flag) {

    char *d = reinterpret_cast<char*>(buffer);

    int __l = sizeof(sockaddr_in);
    if( __state == Connected ) {
        if( (rb = recv(__socket, d, bufferSize, flag)) < 0 ) {
            if( DXT_LAST_ERROR == DXT_EMSGSIZE ) {
                DL_ERROR(1, "Message size more than buffer size!");
            } else {
                DL_ERROR(1, "recv fault");
            }
        }
    } else {
        if( (rb = recvfrom(__socket, d, bufferSize, flag, __updClient, &__l)) < 0 ) {
            if( DXT_LAST_ERROR == DXT_EMSGSIZE ) {
                DL_ERROR(1, "Message size more than buffer size!");
            } else {
                DL_ERROR(1, "recvfrom fault");
            }
        }
    }

    return rb;
}
bool DXT::blockingAccept(int port) {

    if( __type == UDP ) {
        DL_WARNING(1, "accept for TCP type");
        return false;
    }
    if( __state != Empty ) {
        DL_ERROR(1, "Socket in [%s] state, stop it", stateName_c());
        return false;
    }
    if( !DXT_CHECK_PORT(port) ) {
        DL_BADVALUE(1, "port");
        return false;
    }
    if( dxt_global_server.acceptIncoming(port, 1) ) {
        boundSocket s;
        if( dxt_global_server.getAccepted(s)  == false ) {
            DL_FUNCFAIL(1, "getAccepted");
            return false;
        }
        __socket = s.socket;
        __state = Accepted;
        __boundAddress.set(port);
        __remoteAddress = DXTUtil::extractPeerAddress(__socket);
        if( !__remoteAddress.isValid() ) {
            DL_WARNING(1, "Can't extract peer address");
        }
        return true;
    }
    return false;
}
bool DXT::blockingAccept(int port, int timeout_s) {

    if( timeout_s < 1 ) return accept(port);

    if( __type == UDP ) {
        DL_WARNING(1, "accept for TCP type");
        return false;
    }
    if( __state != Empty ) {
        DL_ERROR(1, "Socket in [%s] state, stop it", stateName_c());
        return false;
    }
    if( !DXT_CHECK_PORT(port) ) {
        DL_BADVALUE(1, "port");
        return false;
    }

    struct timeval tv1, tv2;
    int diff = 0;

    tv1 = PROFILER::gettime();
    while( diff < timeout_s ) {

        if( dxt_global_server.checkIncomingConnection(port) ) {

            if( dxt_global_server.acceptIncoming(port, 1) ) {
                boundSocket s;
                if( dxt_global_server.getAccepted(s)  == false ) {
                    DL_FUNCFAIL(1, "getAccepted");
                    return false;
                }
                __socket = s.socket;
                __state = Accepted;
                __boundAddress.set(port);
                __remoteAddress = DXTUtil::extractPeerAddress(__socket);
                if( !__remoteAddress.isValid() ) {
                    DL_WARNING(1, "Can't extract peer address");
                }
                return true;
            }

        }
        tv2 = PROFILER::gettime();
        diff = PROFILER::sec_dif(&tv1, &tv2);
    }


    return false;
}

bool DXT::__createSocket() {

    int type    =  (__type == TCP)  ? SOCK_STREAM : SOCK_DGRAM;
    int proto   =  (__type == UDP)  ? IPPROTO_UDP : 0;
    if( (__socket = DXTUtil::socket(AF_INET, type, proto)) == INVALID_SOCKET) {
        DL_ERROR(1, "Can't create socket");
        return false;
    }
    if( __type == UDP ) {
        int l = sizeof(int);
        getsockopt(__socket, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)&maxDatagramSize, &l);
    }
    return true;
}
void DXT::__clear() {
    __socket = INVALID_SOCKET;
    __state = Empty;
    maxDatagramSize = 0;
    lastReceiveBuffer = nullptr;
    rb = 0;
    total_rb = 0;
    lastSendBuffer = nullptr;
    sb = 0;
    total_sb = 0;
    __auto_dc = true;
}
void DXT::setUDPTarget(const DXTUtil::DXTAddress &a) { __udpTarget = a; }
int DXT::__extractLocalPort() const {

    if( DXTUtil::invalid_socket(__socket) ) {
        return -1;
    }
    return __boundAddress.port();
}
const char *DXT::__extractLocalAddress() const {

    if( DXTUtil::invalid_socket(__socket) ) {
        return nullptr;
    }
    return __boundAddress.address();
}
int DXT::__extractPeerPort() const {

    if( DXTUtil::invalid_socket(__socket) ) {
        return -1;
    }
    if( __type == UDP && __state != Connected ) return __udpTarget.port();
    return __remoteAddress.port();
}
const char *DXT::__extractPeerAddress() const {

    if( DXTUtil::invalid_socket(__socket) ) {
        return nullptr;
    }
    if( __type == UDP && __state != Connected ) return __udpTarget.address();
    return __remoteAddress.address();
}


void exapmple__tcp_server() {

    DXTServer server;
    // open port:
    server.create(65535);

    // check and accept incoming connection:
    boundSocket newConnection;
    while(true) {
        if( server.checkIncomingConnection(65535) ) {
            if( server.acceptIncoming(65535) ) {
                if( server.getAccepted(newConnection) ) {
                    // success
                }
            }
        }
    }
    // create instance:
    DXT c(newConnection);

    //-------------------------------------------- or use global server by default:

    DXT c2(DXT::TCP);
    c2.makeServer(65535);
    c2.accept(65535);
}
void exapmple__tcp_client() {

    // create instance:
    DXT c(DXT::TCP);

    /// unnecessary
    // bind to local address:
    c.bind(49001);

    // or auto bind implicit in connect
    c.connect(65535, "127.0.0.1");
}
void example__udp_server() {

    // create instance:
    DXT c(DXT::UDP);

    // bind to local address:
    c.bind(65535);

    /// unnecessary
    // "bind" to remote address:
    c.connect(49001, "127.0.0.1");
}
void example__udp_client() {

    // create instance:
    DXT c(DXT::UDP);

    // bind to local address:
    c.bind(65535);

    /// unnecessary
    // "bind" to remote address:
    c.connect(49001, "127.0.0.1");
}
