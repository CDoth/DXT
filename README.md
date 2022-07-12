# DXT

DXT is TCP/UDP connector based on sockets. Another words it is TCP/UDP socket C++ wrapper.

## Getting start

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
