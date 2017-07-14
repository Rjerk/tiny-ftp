#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "noncopyable.h"
#include "Socket.h"

#include <memory>

namespace ftpclient {

class InetAddr;

class TcpStream;
using TcpStreamPtr = std::unique_ptr<TcpStream>;

class Acceptor : noncopyable {
public:
    explicit Acceptor(const InetAddr& listen_addr);
    ~Acceptor() = default;
    Acceptor(Acceptor&&) = default;
    Acceptor& operator=(Acceptor&&) = default;

    TcpStreamPtr accept();
private:
    Socket listen_sock;
};


}

#endif
