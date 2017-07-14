#include "ftpClient.h"
#include "error.h"

int main(int argc, char** argv)
{
    bool validConnectRequest;
    string IP;
    int PORT = 21;
    if (argc == 1) {
        validConnectRequest = false;
    } else if (argc == 2) {
        validConnectRequest = true;
        IP = string(argv[1]);
    } else if (argc == 3) {
        validConnectRequest = true;
        IP = string(argv[1]);
        PORT = atoi(argv[2]);
    } else {
        error_Msg("Usage: ./ftp host-name [port]");
        validConnectRequest = false;
    }

    ftpClient client(IP, PORT);
    if (validConnectRequest)
        client.runClient();

    while (client.isRunning()) {
        client.runPI();
    }

    return 0;
}
#include "ftpClient.h"
#include "error.h"

int main(int argc, char** argv)
{
    bool validConnectRequest;
    string IP;
    int PORT = 21;
    if (argc == 1) {
        validConnectRequest = false;
    } else if (argc == 2) {
        validConnectRequest = true;
        IP = string(argv[1]);
    } else if (argc == 3) {
        validConnectRequest = true;
        IP = string(argv[1]);
        PORT = atoi(argv[2]);
    } else {
        error_Msg("Usage: ./ftp host-name [port]");
        validConnectRequest = false;
    }

    ftpClient client(IP, PORT);
    if (validConnectRequest)
        client.runClient();

    while (client.isRunning()) {
        client.runPI();
    }

    return 0;
}
