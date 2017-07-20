#ifndef FTP_SERVER_H
#define FTP_SERVER_H

#include "TcpStream.h"
#include "Acceptor.h"

namespace ftp {

namespace ftpserver {

class FtpServer : noncopyable {
public:
	explicit FtpServer(int port);
	~FtpServer();
	void run();
	
private:
	int accept();
	void closeConn();
	
private:
	TcpStreamPtr stream;
	Acceptor acceptor;
	int port;
};

}

}


#endif 
