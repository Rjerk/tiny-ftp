#include "TcpStream.h"

#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>

namespace ftpclient {

namespace {

class IgnoreSigPipe {
public:
    IgnoreSigPipe()
    {
        ::signal(SIGPIPE, SIG_IGN);
    }
} initObj;

bool isSelfConnection(const Socket& sockfd)
{
    return sockfd.getLocalAddr() == sockfd.getPeerAddr();
}

}

TcpStream::TcpStream(Socket&& sock_)
    : sock(std::move(sock_))
{
}

int TcpStream::receiveAll(void* buf, int len)
{
    // FIXME: EINTR
    return ::recv(sock.fd(), buf, len, MSG_WAITALL);
}

int TcpStream::receiveSome(void* buf, int len)
{
    // FIXME: EINTR
    return sock.read(buf, len);
}

int TcpStream::sendAll(const void* buf, int len)
{
    int written = 0;
    while (written < len) {
        int nw = sock.write(static_cast<const char*>(buf)+written, len-written);
        if (nw > 0) {
            written += nw;
        } else if (nw == 0) {
            break;
        } else if (errno != EINTR) {
            break;
        }
    }
    return written;
}

int TcpStream::sendSome(const void* buf, int len)
{
    // FIXME: EINTR
    return sock.write(buf, len);
}

void TcpStream::setTcpNoDelay(bool on)
{
    sock.setTcpNoDelay(on);
}

void TcpStream::shutdownWrite()
{
    sock.shutdownWrite();
}

TcpStreamPtr TcpStream::connect(const InetAddr& servaddr)
{
    return connectInternal(servaddr, nullptr);
}

TcpStreamPtr TcpStream::connect(const InetAddr& servaddr, const InetAddr& localaddr)
{
    return connectInternal(servaddr, &localaddr);
}

TcpStreamPtr TcpStream::connectInternal(const InetAddr& servaddr, const InetAddr* localaddr)
{
    TcpStreamPtr stream;
    Socket sockfd(Socket::createTCP());
    if (localaddr) {
        sockfd.bindOrDie(*localaddr);
    }
    if (sockfd.connect(servaddr) == 0 && !isSelfConnection(sockfd)) {
        // FIXME: do poll(POLLOUT) to check errors.
        stream.reset(new TcpStream(std::move(sockfd)));
    }
    return stream;
}

}
