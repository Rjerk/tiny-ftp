#ifndef USER_DTP_H
#define USER_DTP_H

#include "noncopyable.h"
#include "InetAddr.h"
#include "TcpStream.h"

#include <string>

using std::string;

namespace ftpclient {

class UserDTP : noncopyable {
public:
    explicit UserDTP();
    ~UserDTP();
    void accept(int16_t port);
    static void reciveData(int16_t port);
    static void sendData();
private:
    TcpStreamPtr stream;
};

}

#endif
