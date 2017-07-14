#ifndef USER_PI_H
#define USER_PI_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <poll.h>
#include <unistd.h>
#include <cstring>

#include "error.h"
#include "utility.h"
#include "noncopyable.h"
#include "InetAddr.h"
#include "TcpStream.h"

using std::string;
using std::cin;
using std::endl;
using std::cout;
using std::vector;

namespace ftpclient {

class UserPI : noncopyable {
public:
    explicit UserPI();
    ~UserPI();
    void connect(const string& ip, int16_t port);
    static void sendCommand();
    static void getReply();
private:
    TcpStreamPtr stream;
};

}

#endif
