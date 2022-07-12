#ifndef DXTSERVER_H
#define DXTSERVER_H
#include "DXTUtil.h"

#include <DArray.h>
#include <map>


struct boundSocket {
    boundSocket() {
        clear();
    }
    void clear() {
        socket = INVALID_SOCKET;
    }
    SOCKET socket;
};
class DXTServer {
public:
    DXTServer();
    ~DXTServer();
public:
    bool isOpen(int port);
    bool create(int port);
    bool acceptIncoming(int port, int block = 0);
    bool checkIncomingConnection(int port);
    bool getAccepted(boundSocket &inLastAccepted);
private:

    // [port, socket] map
    std::map<u_short, boundSocket> socket_map;
    DArray<boundSocket> acceptedQueue;
};

#endif // DXTSERVER_H
