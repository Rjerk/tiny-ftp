#include "FtpServer.h"
#include "Acceptor.h"
#include "ClientConn.h"
#include "error.h"

#include <functional>
#include <sys/types.h>
#include <pthread.h>
#include <thread>

using namespace ftp;
using namespace ftpserver;

void* handleClientConn(void* conn)
{
	ClientConn* data = static_cast<ClientConn*>(conn); 
	data->handleRequest();
	delete data;
	return NULL;
}

//void handleClientConn(ClientConn* conn)
//{
//	conn->handleRequest();
//}

FtpServer::FtpServer(int port_)
	: stream(nullptr), acceptor{InetAddr(port_)}, port(port_)
{
}

FtpServer::~FtpServer()
{
	closeConn();
}

void FtpServer::run()
{
	int connsock;
	if ((connsock = accept()) != -1) {
		std::cout << "connsock: " << connsock << "\n";
		pthread_t thread_id;
		auto conn = new ClientConn(Socket(connsock));
//		std::thread t(handleClientConn, conn);
//		t.join();
		if (pthread_create(&thread_id, NULL, &handleClientConn, conn)) {
			delete conn;
		    error_Exit("failed to create thread.");
		}
	}
}

int FtpServer::accept()
{
    stream = acceptor.accept();
    if (!stream) {
        return -1;
    }
    return stream->getSock();
}

void FtpServer::closeConn()
{
	if (stream)
		stream->closeConn();
}
