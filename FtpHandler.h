#ifndef FTP_HANDLER_H
#define FTP_HANDLER_H

#include "UserPI.h"
#include "UserDTP.h"

namespace ftpclient {

class FtpHandler : noncopyable {
public:
    explicit FtpHandler(std::string ip, int16_t port);

    void connect();
    void login();
    void buildControlConnection();
    void buildDataTrassfer(); 
private:
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
    void Disconnect() { is_connected = false; up.closeConn(); ud.closeConn(); } // undo : fix disconnected.
    bool checkConnected();
private:
    UserPI up;
    UserDTP ud;
    std::string ip_;
    int16_t port_;
    volatile bool is_connected;
    volatile bool is_running; 
    volatile bool is_pasv_conn;
    string cmd;
    std::vector<string> instructions;
};


}

#endif
