#ifndef DXTUTIL_H
#define DXTUTIL_H
#include "dxt_global.h"
namespace DXTUtil {

    struct DXTAddress {
    public:
        DXTAddress();
        explicit DXTAddress(int port, const char *address = nullptr);
        explicit DXTAddress(const sockaddr_in &a);

        bool set(int port, const char *address = nullptr);
        void clear();

        bool isValid() const;


        int port() const;
        const char * address() const;

        inline sockaddr_in & inner() {return __address;}
        inline const sockaddr_in & inner() const {return __address;}

        inline operator sockaddr * () { return reinterpret_cast<sockaddr*>(&__address); }
        inline operator const sockaddr * () const { return reinterpret_cast<const sockaddr*>(&__address); }
    private:
        sockaddr_in __address;
    };

    enum nums {
        sockaddr_in__len = sizeof(sockaddr_in)
    };

    inline DXTAddress createAddress(int port, const char *address) { return DXTAddress(port, address); }
    DXTAddress extractLocalAddress(SOCKET socket);
    DXTAddress extractPeerAddress(SOCKET socket);

    int port(const sockaddr_in &a);
    const char *address(const sockaddr_in &a);
    SOCKET  socket(int af, int type, int protocol);
    SOCKET  accept(SOCKET s, sockaddr *addr, int *addrlen);
    bool    setStats(sockaddr_in &in, int port, const char *address = nullptr);
    bool    is_valid_socket(SOCKET s);
    bool    invalid_socket(SOCKET s);
    bool    bind(SOCKET s, const sockaddr *addr);
    bool    listen(SOCKET s, int backlog);
    bool    connect(SOCKET s, const sockaddr *name, int namelen);
    bool    shutdown(SOCKET s, int how);
    bool    closesocket(SOCKET s);
    int     select(SOCKET socket, fd_set *read_set, fd_set *write_set, fd_set *err_set, struct timeval *tv);

}


#endif // DXTUTIL_H
