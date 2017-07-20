#include "InetAddr.h"

#include <cassert>
#include <cstring>
#include <netdb.h>
#include <strings.h> // bzero
#include <arpa/inet.h>

using namespace ftp;

InetAddr::InetAddr(uint16_t port, bool loop_back_only)
{
    ::bzero(&saddr_, sizeof(saddr_));
    saddr_.sin_family = AF_INET;
    saddr_.sin_addr.s_addr = htonl(loop_back_only ? INADDR_LOOPBACK : INADDR_ANY);
    saddr_.sin_port = htons(port);
}

std::string InetAddr::toIp() const
{
    char buf[32] = "";
    ::inet_ntop(AF_INET, &saddr_.sin_addr, buf, sizeof(buf));
    return buf;
}

std::string InetAddr::toIpPort() const
{
    char buf[32] = "";
    ::inet_ntop(AF_INET, &saddr_.sin_addr, buf, sizeof(buf));
    int end = ::strlen(buf);
    uint16_t port = portHostEndian();
    snprintf(buf+end, sizeof(buf)-end, ":%u", port);
    return buf;
}

static const int kResolveBufSize = 4096; // RFC6891: EDNS playload 4096 bytes.

bool InetAddr::resolveSlow(const char* hostname, InetAddr* out)
{
    std::vector<char> buf(2 * kResolveBufSize);
    struct hostent hent;
    struct hostent* he = NULL;
    int herrno = 0;
    bzero(&hent, sizeof(hent));

    while (buf.size() <= 16 * kResolveBufSize) {
        int ret = gethostbyname_r(hostname, &hent, buf.data(), buf.size(), &he, &herrno);
        if (ret == 0 && he != NULL) {
            assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
            out->saddr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
            return true;
        } else if (ret == ERANGE) {
            buf.resize(buf.size() * 2);
        } else {
            if (ret) {
                perror("InetAddr::resolveSlow");
            }
            return false;
        }
    }
    return false;
}

bool InetAddr::resolve(std::string hostname, InetAddr* out)
{
    assert(out);
    char buf[kResolveBufSize];
    struct hostent hent;
    struct hostent* he = NULL;
    int herror = 0;
    bzero(&hent, sizeof(hent));

    int ret = gethostbyname_r(hostname.c_str(), &hent, buf, sizeof(buf), &he, &herror);
    if (ret == 0 && he != NULL) {
        assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
        out->saddr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        return true;
    } else if (ret == ERANGE) {
        return resolveSlow(hostname.c_str(), out);
    } else {
        if (ret) {
            perror("InetAddress::resolve");
        }
        return false;
    }
}

