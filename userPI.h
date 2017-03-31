#ifndef USER_PI_H
#define USER_PI_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <poll.h>
#include <unistd.h>
#include <cstring>

#include "error.h"
#include "utility.h"

using std::string;
using std::cin;
using std::endl;
using std::cout;
using std::vector;

class userPI {
public:
	int getReplyFromServer();
    string getReplyMessage();
    int getReplyCode();
    bool loginServer();
    int sendServerCmd(const string&);
    void getPasv();
    bool pasvReady();
    int getAsciiMsgFromServer();
    int receiveFile(int);
    int sendFile(int);
    string getAsciiMsg();
    void Configure(string, int);
private:
    int openPasvSock(int);
    int getPort(const string&);
private:
	string replymsg;
	int replyCode;
	string asciiMsg;
	bool isPasv;
    int pasvSock;
	int clntSock;
	string ip;
};

#endif
