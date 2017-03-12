#include "ftpClient.h"

int main(int argc, char** argv)
{
    if (argc > 3)
        error_Exit("Usage: ./ftp hostname [port]");

    string IP;
    int Port = -1;
    if (argc == 2) {
        IP = string(argv[1]);
        Port = 21;
    }
    if (argc == 3) {
        IP = string(argv[1]);
        Port = atoi(argv[2]);
    }

    ftpClient client(IP, Port);
    client.runClient();
    while (true)
        client.runCommand();

    return 0;
}
