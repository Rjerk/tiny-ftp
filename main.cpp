#include "FtpHandler.h"
#include <cstdlib>

using namespace ftpclient;

int main(int argc, char* argv[])
{
    if (argc == 1 || argc >= 4) {
        if (argc >= 4) {
            error_Msg("usage: ftp host-name [port]");
        }
        FtpHandler handler;
        handler.runShell();
        return 0;
    } else if (argc == 2){
        FtpHandler handler{string(argv[1])};
        handler.connectAndRun();
    } else {
        FtpHandler handler{string(argv[1]), static_cast<int16_t>(atoi(argv[2]))};
        handler.connectAndRun();
    }
}
