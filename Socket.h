#ifndef SOCKET_H
#define SOCKET_H

#include "noncopyable.h"

#include <unistd.h>
#include <cassert>
#include <cstring>

#include <utility>
#include <iostream>

namespace ftp {

class InetAddr;

class Socket : noncopyable {
public:
    explicit Socket(int sockfd)
        : sockfd_(sockfd)
    {
        assert(sockfd_ >= 0);
    }

    ~Socket()
    {
    }

    Socket(Socket&& rhs)
        : Socket(rhs.sockfd_)
    {
    }

    Socket& operator=(Socket&& rhs)
    {
        swap(rhs);
        return *this;
    }

    void swap(Socket& rhs)
    {
        std::swap(sockfd_, rhs.sockfd_);
    }

    int fd() const { return sockfd_; }

    void bindOrDie(const InetAddr& localaddr);
    void listenOrDie();
    int connect(const InetAddr& peeraddr);

    void shutdownWrite();

    void setReuseAddr(bool on);
    void setTcpNoDelay(bool on);

    InetAddr getLocalAddr() const;
    InetAddr getPeerAddr() const;

    int read(void* buf, int len);
    int write(const void* buf, int len);

    static Socket createTCP();
	void closeConn();
private:
    int sockfd_;
};

}

#endif
