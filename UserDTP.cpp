#include "UserDTP.h"
#include "Acceptor.h"

namespace ftpclient {

UserDTP::UserDTP()
{
}

UserDTP::~UserDTP()
{

}

void UserDTP::accept(int16_t port)
{
    Acceptor acceptor{InetAddr(port)};
    stream = acceptor.accept();
    if (!stream) {
        return ;
    }
}

void UserDTP::reciveData(int16_t port)
{

}

void UserDTP::sendData()
{

}


}
