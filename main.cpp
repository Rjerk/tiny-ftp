#include "FtpHandler.h"
using namespace ftpclient;

int main()
{
    FtpHandler handler("localhost", 21);
    handler.connect();
    handler.login();
    handler.runShell();
}
