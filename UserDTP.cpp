#include "UserDTP.h"
#include "Acceptor.h"

namespace ftpclient {

UserDTP::UserDTP()
	: pasv_sock(-1)
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
    return pasv_sock;
}

void UserDTP::reciveData(int16_t port)
{

}

void UserDTP::sendData()
{

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
