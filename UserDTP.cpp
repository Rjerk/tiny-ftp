#include "UserDTP.h"
#include "Acceptor.h"

namespace ftpclient {

UserDTP::UserDTP()
	: pasv_sock(-1), is_pasv_ready(false)
{
}

UserDTP::~UserDTP()
{

}

int UserDTP::accept(int16_t port)
{
    Acceptor acceptor{InetAddr(port)};
    stream = acceptor.accept();
    if (!stream) {
        return -1;
    }
    pasv_sock = stream->getSock();
    is_pasv_ready = true;
    return pasv_sock;
}

void UserDTP::reciveData(void* buf, int len)
{
	stream->receiveAll(buf, len);
}

void UserDTP::sendData(const void* buf, int len)
{
	stream->sendAll(buf, len);
}

int UserDTP::openPasvSock(int port)
{
	return accept(port);
}

void UserDTP::closeConn()
{
	stream->closeConn();
}

}
