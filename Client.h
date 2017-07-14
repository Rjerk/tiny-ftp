#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>       // cin, cout, cend
#include <string>         // string
#include <cstring>        // strtok
#include <unordered_map>  // unordered_map
#include <vector>
#include <sstream>
#include <iomanip>        // setprecision

#include <sys/time.h>     // timeval
#include <unistd.h>       // read, write, close, getlogin_r
#include <wait.h>

#include "userFTP.h"
#include "error.h"
#include "utility.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::vector;

namespace ftpclient {

class Client : noncopyable {
public:
    Client(string, int);
    ~Client();
    void runClient();
    bool isRunning();
    void runPI();
    bool isConnected();
    void Disconnect();
private:
    void init();
    void cmd_help();
    void cmd_get();
    void cmd_put();
    void cmd_ls();
    void cmd_pwd();
    void cmd_open();
    void cmd_close();
    void cmd_quit();
    void cmd_cd();
    void cmd_delete();
    void cmd_system();
private:
    string ip;
    int port;
    bool running;
    bool connected;
    bool login;
    string cmd;
    int clntSock;
	vector<string> instructions;
private:
	userFTP pi;
};

}

#endif
