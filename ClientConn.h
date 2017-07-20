#ifndef CLIENT_CONN_H
#define CLIENT_CONN_H

#include "noncopyable.h"
#include "Socket.h"

#include <string>

using std::string;

namespace ftp {

namespace ftpserver {

class ClientConn : noncopyable {
public:
	ClientConn(Socket&& sock_);
	~ClientConn();
	void handleRequest();
private:
	void handleUserCmd(const string& request);
	void handlePassCmd(const string& request);
	void handlePasvCmd(const string& request);
	void handleDeleCmd(const string& request);
	void handleSystCmd(const string& request);
	void handleTypeCmd(const string& request);
	void handleRetrCmd(const string& request);
	void handleStorCmd(const string& request);
	void handleListCmd(const string& request);
	void handleQuitCmd(const string& request);
	void handlePwdCmd (const string& request);
	void handleCwdCmd (const string& request);
private:
	Socket conn_sock;
	volatile bool is_connected;
}; 

}

}
#endif
