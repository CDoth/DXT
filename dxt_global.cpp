#include "dxt_global.h"
#include <iostream>

#ifdef WIN32
void dxt_start() {
    char buff[1024];
    WSAStartup(0x202,(WSADATA *)&buff[0]);
}
std::string GetLastErrorAsString() {
    //source:
    //https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror

    {
        //Get the error message ID, if any.
        DWORD errorMessageID = ::GetLastError();
        if(errorMessageID == 0) {
            return std::string(); //No error message has been recorded
        }

        LPSTR messageBuffer = nullptr;

        //Ask Win32 to give us the string version of that message ID.
        //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                     NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

        //Copy the error message into a std::string.
        std::string message(messageBuffer, size);
        message.pop_back();
        message.pop_back();

        //Free the Win32's string's buffer.
        LocalFree(messageBuffer);

        return message;
    }
}
#endif
std::string dxt_error_description() {

#ifdef WIN32
    return  GetLastErrorAsString();
#else
    return strerror(errno);
#endif
    return nullptr;
}
void dxt_print_error(const char *func_name) {

    std::cout << "DXT Error:"
              << " Call: [" << func_name << "]"
              << " Code: [" << DXT_LAST_ERROR << "]"
              << " Description: [" << dxt_error_description() << "]"
              << std::endl;
}
void dxt_print_inner_error(const char *func_name, const char *desc) {

    std::cout << "DXT Inner Error:"
              << " Call: [" << func_name << "]"
              << " Description: [" << desc << "]"
              << std::endl;
}
void dxt_print_info(const char *func_name, const char *info) {

    std::cout << "DXT:"
              << " Call: [" << func_name << "]"
              << " Info: [" << info << "]"
              << std::endl;
}

DLogs::DLogsContext DXTLogsNamespace::log_context;
DLOGS_INIT_GLOBAL_CONTEXT("DXT", DXTLogsNamespace::logsInit);
