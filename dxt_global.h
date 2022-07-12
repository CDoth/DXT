#ifndef DXT_GLOBAL_H
#define DXT_GLOBAL_H
//======================================================
#include <string>
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else

#define SOCKET int
#define INVALID_SOCKET (-1)
#define SD_BOTH SHUT_RDWR
#define SOCKET_ERROR INVALID_SOCKET

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring> //memset
#include <unistd.h> //close
#endif
//====================================================== error codes:
#ifdef _WIN32
#define DXT_WOULD_BLOCK WSAEWOULDBLOCK
#define DXT_EISCONN WSAEISCONN
#define DXT_EADDRINUSE WSAEADDRINUSE
#define DXT_EMSGSIZE WSAEMSGSIZE
#define DXT_EADDRINUSE WSAEADDRINUSE
#else
#define DXT_WOULD_BLOCK EWOULDBLOCK
#define DXT_EISCONN EISCONN
#define DXT_EADDRINUSE EADDRINUSE
#define DXT_EMSGSIZE EMSGSIZE
#define DXT_EADDRINUSE EADDRINUSE
#endif
//====================================================== last error:
#ifdef _WIN32
#define DXT_LAST_ERROR WSAGetLastError()
#else
#define DXT_LAST_ERROR (errno)
#endif
//====================================================== print:
#define DXT_PRINT_ERRORS
#define DXT_PRINT_INFO

#ifdef DXT_PRINT_ERRORS
    #define DXT_ERROR dxt_print_error(__func__);
#else
    #define DXT_ERROR
#endif
#ifdef DXT_PRINT_INFO
    #define DXT_INFO(Info) dxt_print_error(__func__, Info);
#else
    #define DXT_INFO(Info)
#endif

void dxt_print_error(const char *func_name);
void dxt_print_inner_error(const char *func_name, const char *desc);
void dxt_print_info(const char *func_name, const char *info);
//====================================================== winsock:
#ifdef _WIN32
void dxt_start();
std::string GetLastErrorAsString();
#endif
std::string dxt_error_description();
//======================================================
#define DXT_CHECK_PORT(port) (port > 0 && port <= 65535)

#include <DLogs.h>
namespace DXTLogsNamespace {
extern DLogs::DLogsContext log_context;
extern DLogs::DLogsContextInitializator logsInit;
}





#endif // DXT_GLOBAL_H
