#ifndef USER_DTP_H
#define USER_DTP_H

#include "noncopyable.h"
#include "InetAddr.h"
#include "TcpStream.h"

#include <string>

using std::string;

namespace ftpclient {

class UserDTP : noncopyable {
public:
    explicit UserDTP(const string& ip_, int16_t port_);
    ~UserDTP();
    int connect(const string& ip_, int16_t port_);
    void reciveData(void* buf, int len);
    void sendData(const void* buf, int len);

	int getAsciiMsgFromServer();
	string getAsciiMsg();
    int receiveFile(int);
    int sendFile(int);

	int openPasvSock(const string& ip_, int port_);
	void closeConn();
private:
    TcpStreamPtr stream;
    string ip;
    string ascii_msg;
    int16_t port;
    int pasv_sock;
    bool is_pasv_ready;
};

}

#endif
