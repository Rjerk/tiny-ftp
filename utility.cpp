#include "utility.h"

int Connect(string _ip, int _port)
{
	struct hostent* host = gethostbyname(_ip.c_str());
    sockaddr_in clntSockAddr;
    bzero((void*) &clntSockAddr, sizeof(clntSockAddr));
    clntSockAddr.sin_family = AF_INET;
    clntSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    clntSockAddr.sin_port = htons(_port);

    int _clntSock = socket(AF_INET, SOCK_STREAM, 0);
    if (_clntSock == -1)
        error_Exit("socket() error!");
    if (connect(_clntSock, (sockaddr*) &clntSockAddr, sizeof(clntSockAddr)) == -1)
        error_Exit("connect() error!");
    return _clntSock;
}

string getUserName()
{
    struct utmp current_record;
    int utmpfd;
    int reclen = sizeof(current_record);

    if ((utmpfd = open(UTMP_FILE, O_RDONLY)) == -1) {
        perror(UTMP_FILE);
        exit(1);
    }

    char* buf = NULL;
    while (read(utmpfd, &current_record, reclen) == reclen) {
        if (current_record.ut_type == USER_PROCESS)
            buf = current_record.ut_name;
    }
    return string(buf);
}

void hideInput()
{
	termios tty;
	tcgetattr(STDIN_FILENO, &tty);
	tty.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void showInput()
{
    termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}
