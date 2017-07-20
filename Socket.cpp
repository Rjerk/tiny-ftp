#include "Socket.h"
#include "InetAddr.h"
#include "utility.h"

#include <strings.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include <iostream>

using namespace ftp;

using SA = struct sockaddr;

const SA* sockaddr_cast(const struct sockaddr_in* addr)
{
    return static_cast<const SA*>(implicit_cast<const void*>(addr));
}

SA* sockaddr_cast(struct sockaddr_in * addr)
{
    return static_cast<SA*>(implicit_cast<void*>(addr));
}

void Socket::bindOrDie(const InetAddr& addr)
{
    const struct sockaddr_in& saddr = addr.getSockAddrInet();
    int ret = ::bind(sockfd_, sockaddr_cast(&saddr), sizeof(saddr));
    if (ret) {
        perror("Socket::bindOrDie");
        abort();
    }
}

void Socket::listenOrDie()
{
    int ret = ::listen(sockfd_, SOMAXCONN);
    if (ret) {
        perror("Socket::listenOrDie");
        abort();
    }
}

int Socket::connect(const InetAddr& addr)
{
    const struct sockaddr_in& saddr = addr.getSockAddrInet();
    return ::connect(sockfd_, sockaddr_cast(&saddr), sizeof(saddr));
}

void Socket::shutdownWrite()
{
    if (::shutdown(sockfd_, SHUT_WR) < 0) {
        perror("Socket::shutdownWrite");
    }
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                    &optval, sizeof(optval)) < 0) {
        perror("Socket::setReuseAddr");
    }
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    if (::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                    &optval, static_cast<socklen_t>(sizeof(optval))) < 0) {
        perror("Socket::setTcpNoDelay");
    }
}

InetAddr Socket::getLocalAddr() const
{
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof(localaddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(localaddr));
    if (::getsockname(sockfd_, sockaddr_cast(&localaddr), &addrlen) < 0) {
        perror("Socket::getLocalAddr");
    }
    return InetAddr(localaddr);
}

InetAddr Socket::getPeerAddr() const
{
    struct sockaddr_in peeraddr;
    bzero(&peeraddr, sizeof(peeraddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(peeraddr));
    if (::getpeername(sockfd_, sockaddr_cast(&peeraddr), &addrlen) < 0) {
        perror("Socket::getPeerAddr");
    }
    return InetAddr(peeraddr);
}

int Socket::read(void* buf, int len)
{
    // FIXME: EINTR
    int s = ::read(sockfd_, buf, len);
    return s;
}

int Socket::write(const void* buf, int len)
{
    // FIXME: EINTR
    return ::write(sockfd_, buf, len);
}

Socket Socket::createTCP()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
    assert(sockfd >= 0);
    return Socket(sockfd);
}

void Socket::closeConn()
{
	if (::close(sockfd_)) {
		perror("Socket::closeConn");
	}
	sockfd_ = -1;
}

