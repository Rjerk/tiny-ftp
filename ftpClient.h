#include "error.h"
#include <iostream>       // cin, cout, cend
#include <string>         // string
#include <cstring>        // strtok
#include <map>            // map
#include <vector>
#include <sstream>

#include <cstdio>         // perror
#include <utmp.h>         // stuct utmp
#include <fcntl.h>        // open
#include <sys/time.h>     // timeval
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close, getlogin_r
#include <string.h>       // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <sys/wait.h>     // wait
#include <poll.h>         // pollfd

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::vector;
using std::map;

class ftpClient {
public:
    // friend class Shell;
    ftpClient();
    ftpClient(string, int);
    ~ftpClient();
    void runClient();
    void runCommand();
private:
    void getPasv();
    bool pasvReady();
    int getAsciiMsgFromServer();
    string getAsciiMsg();
    int openPasvSock(int);
	void init() const;
    bool isConnected();
    int Connect(const string&, int);
    void disconnect();
    int getReplyFromServer();
    int getReplyCode();
    string getReplyMessage();
    int getPort(const string&);
    void loginServer();
    int sendServerCmd(const string&);
    string getUserName();
    void parseCmd();
private:
    void cmd_help();
    void cmd_get();
    void cmd_put();
    void cmd_ls();
    void cmd_pwd();
    void cmd_open();
    void cmd_close();
    void cmd_quit();
    void cmd_cd();
	bool isConn;
    bool isPasv;
    int pasvSock;
    string asciiMsg;
    string cmd;
    vector<string> instructions;
private:
    string ip;
    int port;
    int clntSock;
    bool islogin;
    int replyCode;
    string replymsg;
};
