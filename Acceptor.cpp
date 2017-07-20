#include "Acceptor.h"
#include "TcpStream.h"
#include <sys/socket.h>
#include <iostream>

using namespace ftp;

Acceptor::Acceptor(const InetAddr& listen_addr)
    : listen_sock(Socket::createTCP())
{
    listen_sock.setReuseAddr(true);
    listen_sock.bindOrDie(listen_addr);
    listen_sock.listenOrDie();
}

TcpStreamPtr Acceptor::accept()
{
    // FIXME: use accept4
    int sockfd = ::accept(listen_sock.fd(), NULL, NULL);
    if (sockfd >= 0) {
        return TcpStreamPtr(new TcpStream(Socket(sockfd)));
    } else {
        perror("Acceptor::accept");
        return TcpStreamPtr();
    }
}

