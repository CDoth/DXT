#ifndef DXT_H
#define DXT_H
#include "DXTServer.h"


class DXT {
public:
    enum Type {
        UDP, TCP
    };
    enum InnerState {
        Empty,
        Bound,
        Accepted,
        Connected
    };
    DXT();
    DXT(Type t);
    DXT(boundSocket s);
    ~DXT();
public:

    bool makeServer(int port);
    ///
    /// \brief bind
    /// \param port
    /// \return
    ///  Only in Empty state
    bool bind(int port);
    ///
    /// \brief accept
    /// \param port
    /// \return
    /// Only in Empty state
    bool accept(int port);
    ///
    /// \brief connect
    /// \param port
    /// \param address
    /// \return
    /// Only in Empty or Bound state
    bool connect(int port, const char *address);
    bool newConnection(int port) const;
    bool blockingAccept(int port);
    bool blockingAccept(int port, int timeout_s);
    bool changeType(Type t);
    bool stop();
    void disconnectInDesctructor(bool s) {__auto_dc = s;}
public:
    inline DXTUtil::DXTAddress createUDPTarget(int port, const char *address) const {return DXTUtil::createAddress(port, address);}
    void setUDPTarget(const DXTUtil::DXTAddress &a);
public:
    bool readable(int sec = 0, int usec = 0);
    bool writable(int sec = 0, int usec = 0);
    int send(const void *data, int size, int flag = 0);
    int receive(void *buffer, int size, int flag = 0);
public:
    int localPort() const;
    int peerPort() const;

    inline const char * localAddress_c() const {return __extractLocalAddress();}
    inline const char * peerAddress_c() const {return __extractPeerAddress();}
    const char * stateName_c() const;

    inline std::string localAddress() const {return localAddress_c();}
    inline std::string peerAddress() const {return peerAddress_c();}
    std::string stateName() const;

    Type type() const;
    InnerState state() const;
    bool isBound() const;
    bool isValid() const;
    int errorCode() const;
private:
    int __tcp_send(const void *data, int size, int flag);
    int __udp_send(const void *data, int size, int flag);
    int __tcp_receive(void *place, int bytes, int flag);
    int __udp_receive(void *buffer, int bufferSize, int flag);

    bool __createSocket();
    void __clear();


    int __extractLocalPort() const;
    int __extractPeerPort() const;
    const char * __extractLocalAddress() const;
    const char * __extractPeerAddress() const;
private:
    SOCKET __socket;
    Type __type;
    InnerState __state;
    bool __auto_dc;
private:
    //---------------------------------------------
    int maxDatagramSize;
    DXTUtil::DXTAddress __boundAddress;
    DXTUtil::DXTAddress __remoteAddress;

    DXTUtil::DXTAddress __updClient;
    DXTUtil::DXTAddress __udpTarget;
    //---------------------------------------------
    void *lastReceiveBuffer;
    int rb;
    int total_rb;
    //---------------------------------------------
    const void *lastSendBuffer;
    int sb;
    int total_sb;
    //---------------------------------------------

};

#endif // DXT_H
