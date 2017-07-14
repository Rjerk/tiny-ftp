#ifndef FTP_HANDLER_H
#define FTP_HANDLER_H

#include "UserPI.h"
#include "UserDTP.h"
#include "Socket.h"

#include <string>
#include <unordered_map>

namespace ftpclient {

enum cmdType {
    HELP, GET, PUT, LS,     OPEN, CLOSE,
    QUIT, CD,  PWD, DELETE, SYSTEM
};

std::unordered_map<string, cmdType> cmdMap = {
    { "help", HELP },     { "get", GET },   { "put", PUT },
    { "ls", LS },         { "open", OPEN }, { "close", CLOSE },
    { "quit", QUIT },     { "cd", CD },     { "pwd", PWD },
    { "delete", DELETE }, { "system", SYSTEM }
};

class FtpHandler : noncopyable {
public:
    explicit FtpHandler();

    static void connect(std::string ip, int16_t port);
private:
    bool is_connected;
    UserPI up;
    UserDTP ud;
};


}

#endif
