#ifndef USER_PI_H
#define USER_PI_H

#include <string>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>

#include <unistd.h>

#include "error.h"
#include "utility.h"
#include "noncopyable.h"
#include "TcpStream.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::cerr;
using std::vector;

namespace ftp {

namespace ftpclient {

class UserPI : noncopyable {
public:
    explicit UserPI(const string& ip, int16_t port);
    ~UserPI();

    int    getReplyFromServer();
    string getReplyMessage() { return reply_msg; }
    int    getReplyCode() { return reply_code; }
	bool   loginServer();
	int    sendServerCmd(const string& cmd);

    void connect();
	void closeConn();
	int getPasvPortFromReply(const string&);
    int openPasvSock(int);
	void getPasvConn();
    bool isPasvReady();
private:
    TcpStreamPtr stream;
    string ip;
	int16_t port; 
	int reply_code;
    string reply_msg;
	string ascii_msg;
};

}

}

#endif
