#ifndef TCP_STREAM_H
#define TCP_STREAM_H

#include "noncopyable.h"
#include "InetAddr.h"
#include "Socket.h"

#include <memory>
#include <string>

namespace ftpclient {

class Socket;
class TcpStream;
using TcpStreamPtr = std::unique_ptr<TcpStream>;

class TcpStream : noncopyable {
public:
    TcpStream(Socket&& sock);
    ~TcpStream() = default;
    TcpStream(TcpStream&&) = default;
    TcpStream& operator=(TcpStream&&) = default;

    static TcpStreamPtr connect(const InetAddr& servaddr);
    static TcpStreamPtr connect(const InetAddr& servaddr, const InetAddr& localAddr);

    int receiveAll(void* buf, int len); // read len bytes, unless error happends.
    int receiveSome(void* buf, int len); // read len or less bytes.

    int sendAll(const void* buf, int len); // send len bytes, unless error happends.
    int sendSome(const void* buf, int len); // send len or less bytes.

    void setTcpNoDelay(bool on);
    void shutdownWrite();

	int getSock() const;
private:
    static TcpStreamPtr connectInternal(const InetAddr& serveraddr, const InetAddr* localAddr);
    Socket sock;
};


}

#endif
