#ifndef FTP_HANDLER_H
#define FTP_HANDLER_H

#include "UserPI.h"
#include "UserDTP.h"

namespace ftp {

namespace ftpclient {

class FtpHandler : noncopyable {
public:
    explicit FtpHandler(const string& ip = "localhost", int16_t port = 21);
	~FtpHandler() { }
    void connectAndRun();
    void runShell();
private:
    void connect();
    void login();
    void usePasv();
	void cmd_help();
    void cmd_get();
    void cmd_put();
    void cmd_ls();
    void cmd_pwd();
    void cmd_open();
    void cmd_close();
    void cmd_quit();
    void cmd_cd();
    void cmd_delete();
    void cmd_system();

    bool isConnected() { return is_connected; }
    bool isRunning() { return is_running; }
    void Disconnect() { is_connected = false; up.closeConn(); ud.closeConn(); }
    bool checkConnected();
private:
    UserPI up;
    UserDTP ud;
    std::string ip;
    int16_t port;
    bool is_running;
    bool is_connected;
    string cmd;
    std::vector<string> instructions;
};

}

}

#endif
