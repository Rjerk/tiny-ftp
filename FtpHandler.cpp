#include "FtpHandler.h"
#include "InetAddr.h"

namespace ftpclient {

FtpHandler::FtpHandler()
    : is_connected(false)
{
}

void FtpHandler::connect(std::string ip, int16_t port)
{
    up.connect(ip, port);
    ud.accept(port);
}





}
