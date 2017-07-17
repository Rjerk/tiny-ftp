#include "utility.h"

namespace ftpclient {

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

}
