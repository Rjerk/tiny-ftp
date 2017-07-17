#ifndef USER_PI_H
#define USER_PI_H

#include <string>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>

#include <poll.h>
#include <unistd.h>
 
#include "error.h"
#include "utility.h"
#include "noncopyable.h"
#include "InetAddr.h"
#include "TcpStream.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::cerr;
using std::vector;

namespace ftpclient {

class UserPI : noncopyable {
public:
    explicit UserPI();
    ~UserPI();
    
    int    getReplyFromServer();
    string getReplyMessage() { return reply_msg; }
    int    getReplyCode() { return reply_code; }
	bool   loginServer();
	int    sendServerCmd(const string& cmd);

    void connect(const string& ip, int16_t port);
	void closeConn();
	int getPasvPortFromReply(const string&);
    int openPasvSock(int);
	void getPasvConn();
    bool isPasvReady();
    int receiveFile(int);
    int sendFile(int);
    int getAsciiMsgFromServer();
    string getAsciiMsg();
private:
    void Configure(string, int);
private:
    TcpStreamPtr stream;
    
	int reply_code;
    int pasv_sock;
	int clnt_sock;
	volatile bool is_pasv;
    string reply_msg;
	string ascii_msg;

	string ip;
};

}

#endif
