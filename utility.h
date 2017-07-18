#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <errno.h>
#include <cstdio>
#include "error.h"

namespace ftp {

using std::string;

int Connect(string, int);

string getUserName();

void hideInput();

void showInput();

template <typename To, typename From>
inline
To implicit_cast(const From& f)
{
    return f;
}

}

#endif
