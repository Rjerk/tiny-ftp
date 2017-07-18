#include "UserDTP.h"
#include <poll.h>
#include <unistd.h>

using namespace ftp;
using namespace ftpclient;

UserDTP::UserDTP(const string& ip_, int16_t port_)
	: port(port_), pasv_sock(-1), ip(ip_), ascii_msg("")
{
}

UserDTP::~UserDTP()
{
}

int UserDTP::connect(const string& ip_, int16_t port_)
{
	InetAddr addr(port_);
    if (!InetAddr::resolve(ip_, &addr)) {
        error_Exit("Unable to resolve" + ip_);
        return -1;
    }

    stream = TcpStream::connect(addr);
    if (!stream) {
        error_Exit("Unable to connect " + addr.toIpPort());
        return -1;
    }
    pasv_sock = stream->getSock();
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

int UserDTP::getAsciiMsgFromServer()
{
    struct pollfd ufds;
    ufds.fd = pasv_sock;
    ufds.events = POLLIN; // There is data to read
    ufds.revents = 0;
    ascii_msg.clear();

    char buf[1024];
    int len = 0;
    int total = 0;
    while (poll(&ufds, 1, 1000) > 0) {
        bzero(buf, sizeof(buf));
        len = read(pasv_sock, static_cast<void*>(&buf), sizeof(buf));
        if (len == 0) break;
        ascii_msg.append(buf);
        total += len;
    }
    closeConn();
    return total;
}

string UserDTP::getAsciiMsg()
{
    return ascii_msg;
}

int UserDTP::openPasvSock(const string& ip_, int port_)
{
	return connect(ip_, port_);
}

int UserDTP::receiveFile(int fd)
{
	struct pollfd ufds;
    ufds.fd = pasv_sock;
    ufds.events = POLLIN; // There is data to read
    ufds.revents = 0;
    ascii_msg.clear();

    char buf[1024];
    int len = 0;
    int total = 0;
    while (poll(&ufds, 1, 1000) > 0) {
        bzero(buf, sizeof(buf));
        if ((len = read(pasv_sock, static_cast<void*>(&buf), sizeof(buf))) == -1) {
        	error_Exit("read() error in receiveFile().");
		}
        if (len == 0) break;
        lseek(fd, 0, SEEK_END);
        if (write(fd, buf, len) == -1) {
        	error_Exit("write() error in receiveFile().");
    		return -1;
		}
        total += len;
    }
    closeConn();
	close(fd);
    return total;
}

int UserDTP::sendFile(int file_sock)
{
	int sz = lseek(file_sock, 0, SEEK_END);
    lseek(file_sock, 0, 0);
    char buf[sz];
    if (read(file_sock, buf, sz) == -1) {
    	error_Exit("read() error in sendFile().");
	}
    close(file_sock);
    int len;
    if ((len = write(pasv_sock, buf, sz)) == -1) {
    	error_Exit("write() error in sendFile().");
	}
    closeConn();
    return len;
}

void UserDTP::closeConn()
{
	stream->closeConn();
}

