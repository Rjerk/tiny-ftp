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
    explicit UserDTP();
    ~UserDTP();
    int accept(int16_t port);
    void reciveData(void* buf, int len);
    void sendData(const void* buf, int len);

	int openPasvSock(int port);
	bool isPasvReady() { return is_pasv_ready; }
	void closeConn();
private:
    TcpStreamPtr stream;
    int pasv_sock;
    bool is_pasv_ready;
};

}

#endif
