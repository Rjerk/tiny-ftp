#ifndef CLIENT_H
#define CLIENT_H

#include "FtpHandler.h"

#include <string>


namespace ftpclient {

class FtpClient : noncopyable {
public:
    explicit FtpClient(const std::string& ip, int16_t port = 21);
    ~FtpClient();
    
    void run(int argc, char* argv[]);
    void connectToServer();
private:
    FtpHandler handler;
    std::string ip_;
    int16_t port_;
};


}

#endif
