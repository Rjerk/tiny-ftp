#ifndef INETADDR_H
#define INETADDR_H

#include "copyable.h"
#include "error.h"

#include <string>
#include <vector>

#include <netinet/in.h>

using std::string;

namespace ftp {

class InetAddr : copyable {
public:
    InetAddr(std::string ip, uint16_t port);
    explicit InetAddr(std::string ip_port); // "1.2.3.4:5678"
    explicit InetAddr(uint16_t port, bool loop_back_only = false);

    explicit InetAddr(const struct sockaddr_in& saddr)
        : saddr_(saddr)
    {
    }

    std::string toIp() const;
    std::string toIpPort() const;

    const struct sockaddr_in& getSockAddrInet() const { return saddr_; }
    void setSockAddrInet(const struct sockaddr_in& saddr) { saddr_ = saddr; }
    void setPort(uint16_t port) { saddr_.sin_port = htons(port); }

    uint32_t ipNetEndian() const { return saddr_.sin_addr.s_addr; }
    uint16_t portNetEndian() const { return saddr_.sin_port; }

    uint32_t ipHostEndian() const { return ntohl(saddr_.sin_addr.s_addr); }
    uint16_t portHostEndian() const { return ntohs(saddr_.sin_port); }

    static bool resolve(std::string hostname, InetAddr*);
    bool operator==(const InetAddr& rhs) const
    {
        return saddr_.sin_family == rhs.saddr_.sin_family
            && ipNetEndian()    == rhs.ipNetEndian()
            && portNetEndian()  == rhs.portNetEndian();
    }

private:
    static bool resolveSlow(const char* hostname, InetAddr*);
    struct sockaddr_in saddr_;
};

}

#endif
