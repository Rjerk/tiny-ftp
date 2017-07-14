#include "UserPI.h"
#include "error.h"
#include "utility.h"

namespace ftpclient {

const int CONTROL_PORT = 21;
const int DATATRANS_PORT = 20;

UserPI::UserPI()
{

}

UserPI::~UserPI()
{

}

void UserPI::connect(const string& ip, int16_t port)
{
    InetAddr addr(port);
    if (!InetAddr::resolve(ip, &addr)) {
        error_Exit("Unable to resolve" + ip);
        return ;
    }

    stream = TcpStream::connect(addr);
    if (!stream) {
        error_Exit("Unable to connect " + addr.toIpPort());
        return ;
    }
}

}
