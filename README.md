# DXT

DXT is TCP/UDP connector based on sockets. Another words it is TCP/UDP socket C++ wrapper.

## Getting start
On windows call `dxt_start()` in `main()` function to init WSA.

## Portability
DXT made for compile on Windows and UNIX systems.
WSA implement sockets mechanic on windows.

## Errors

Compile without 
```
#define DXT_PRINT_ERRORS
#define DXT_PRINT_INFO
```
to ignore error prints

Compile with
```
#define DXT_USE_DLOGS
```
to use `DLogs` for log all errors, warnings and info in DXT.

To get and tune DLogs context go to `DXTLogsNamespace` namespace.

## DXTUtil

Namespace `DXTUtil` contain primitive socket function wrappers (such as `bind`, `connect`, `accept`).
Alose `DXTUtil` contain C++ wrapper for `sockaddr_in` - `DXTAddress`.
Global functions in `DXTUtil` namespace
```
    DXTAddress extractLocalAddress(SOCKET socket);
    DXTAddress extractPeerAddress(SOCKET socket);
```
create `DXTAddress` instance with local or peer address associated with `socket` and return it.

## DXTServer

DXT server using to contain map of TCP listen sockets:
- key - port number
- value - listen socket

With `DXTServer` you can:
- Open new port and make it listening
- Check new incoming tcp conntections on stored ports
- accept incoming connections

After you successful accept new connection you don't get it instantly. After accepting new connection (socket) stored in queue on `DXTServer`.
Use
```
  bool DXTServer::checkIncomingConnection(int port);
```
to check incoming connection on need port.

Use 
```
  bool DXTServer::acceptIncoming(int port, int block = 0)
```
to accept connection.

Use 
```
  bool getAccepted(boundSocket &inLastAccepted);
```
to get accepted connection ( check result, socket are valid only if `getAccepted` returns `true`).

But by default you don't need use `DXTServer` directly. `DXT` instances use global `DXTServer` in its methods to create and check listen ports and for accept new connections.

## DXT

Create DXT instance with `DXT::Type` to point type of socket.
```
    enum Type {
        UDP, TCP
    };
```
Or create it empty and than call `changeType( DXT::Type type )`. Method `changeType` works only on empty DXT objects (unconnected, unbound, uncreated inner socket).

Call `Type type() const` to know type.

## State

DXT object always in its inner state:
```
      enum InnerState {
        Empty,
        Bound,
        Accepted,
        Connected
    };
```
Call `InnerState state() const` to know state or `std::string stateName() const` to get state text alternative.

## Start

> TCP client

For TCP client just call `bool connect(int port, const char *address)` and check result. In this case local port picking automatically (auto binding).
Also you can use `bool bind(int port)` before call `connect` to create inner socket and bind it to requeried port. In this case `connect` will not
bind socket and local port will be requeried. Since you successfully connected - this DXT instance become unavailable for init methods (`bind`, `connect`, `accept`).

> TCP server

For TCP server you have to call `bool makeServer(int port)` to create listening socket in global server map (see `DXTServer`).
After this use pair of methods: `bool newConnection(int port) const` and `bool accept(int port)` to accept new connection which will stored in
this `DXT` instance. Since you successfully accept new connection - this DXT instance become unavailable for init methods (`bind`, `connect`, `accept`).
Also you can use `bool blockingAccept(int port)` and `bool blockingAccept(int port, int timeout_s)` instead default `bool accept(int port)`. This methods block calling thread

> UDP client

For UDP client you can avoid any inition - since you will use `send` first time and socket become automatically bound to local port. But you can bind socket
to requeried port on start using `bool bind(int port)`. This is case of connectionless UDP socket. 
For udp sockets you also can use `bool connect(int port, const char *address)` to create connected UDP socket. Connected socket receive datagrams only from 
connected address and discard others.

> UDP server

For UDP server you have to bind your socket, call `bool bind(int port)`. After this use raw connectionless socket or make it connected (see `UDP client`).


