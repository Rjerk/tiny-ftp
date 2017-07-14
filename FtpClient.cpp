#include "FtpClient.h"

namespace ftpclient {

FtpClient::FtpClient(const std::string& ip, int16_t port)
        : ip_(ip), port_(port)
{

}

FtpClient::~FtpClient()
{

}

void FtpClient::run()
{
    FtpHandler::connect(ip_, port_);

}

}

