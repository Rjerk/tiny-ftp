#include "ftpClient.h"
#include <string>

enum cmdType {
    HELP = 5000, GET, PUT, LS, OPEN, CLOSE, QUIT, CD, PWD
};

map<string, cmdType> cmdMap;


ftpClient::ftpClient()
{
}

ftpClient::ftpClient(string _IP, int _port) :
    ip(_IP), port(_port)
{
	init();
    clntSock = 0;
    replyCode = 0;
    islogin = false;
    isConn = false;
}

ftpClient::~ftpClient()
{
}

void ftpClient::init() const
{
    cmdMap["get"] = GET;
    cmdMap["put"] = PUT;
    cmdMap["ls"] = LS;
    cmdMap["dir"] = LS;
    cmdMap["open"] = OPEN;
    cmdMap["close"] = CLOSE;
    cmdMap["disconnect"] = CLOSE;
    cmdMap["quit"] = QUIT;
    cmdMap["exit"] = QUIT;
    cmdMap["cd"] = CD;
    cmdMap["pwd"] = PWD;
}


int ftpClient::Connect(const string& _ip, int _port)
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
    isConn = true;
    return _clntSock;
}

void ftpClient::disconnect()
{
    close(clntSock);
}

int ftpClient::getReplyFromServer()
{
    replymsg.clear();
    char buf[1024];
    bzero(buf, sizeof(buf));
    int len = read(clntSock, buf, sizeof(buf));
    if (len > 0) replymsg = string(buf);
    // cout << replymsg << endl;
    return len;
}

void ftpClient::runClient()
{
    clntSock = Connect(ip, port);
    cout << "Connected to " << ip << "." << endl;

    if (getReplyFromServer() > 0) {
        // replyMessage: 220 (vsFTPd 3.0.3)
        cout << getReplyMessage();
        replyCode = getReplyCode();
    }

    if (replyCode == 220) {
        loginServer();
        if (islogin) {
            cout << "Remote system type is UNIX." << endl;
            cout << "Using binary mode to transfer files." << endl;
        }
    }
}

int ftpClient::getReplyCode()
{
    if (replymsg.length() > 0) {
        string msg = replymsg;
        char* code = strtok((char*)msg.c_str(), " ");
        return atoi(code);
    }
    return 0;
}

string ftpClient::getReplyMessage()
{
    return replymsg;
}

string ftpClient::getUserName()
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

void ftpClient::loginServer()
{
    cout << "Name (" << ip << ":" << getUserName() << "): ";
    string account;
    string password;
    getline(cin, account);
    string user = "USER " + account;
    if (sendServerCmd(user) > 0) {
        if (getReplyFromServer() > 0) {
            // reply: 331 Please specify the password.
            cout << getReplyMessage();
            replyCode = getReplyCode();
        }
    }
    if (replyCode == 331) {
        cout << "Password: ";
        getline(cin, password);
        string passwd = "PASS " + password;
        if (sendServerCmd(passwd) > 0) {
            if (getReplyFromServer() > 0) {
                // reply: 230 Login Successful.
                cout << getReplyMessage();
                replyCode = getReplyCode();
                if (replyCode == 230)
                    islogin = true;
                else
                    islogin = false;
                return ;
            }
        } else {
            islogin = false;
            return ;
        }
    } else {
        islogin = false;
        return ;
    }
}

int ftpClient::sendServerCmd(const string& cmd)
{
    int len = cmd.length() + 2;
    char buf[len];
    bzero(buf, sizeof(buf));
    strcpy(buf, cmd.c_str());
    strcat(buf, "\r\n");
    return write(clntSock, buf, len);
}

bool ftpClient::isConnected()
{
    return true;
}

void ftpClient::runCommand()
{
    //getPasv();
    cout << "ftp> ";
    getline(cin, cmd);
    vector<string>(instructions).swap(instructions);
    std::stringstream cmdstring(cmd);
    string token;
    while (getline(cmdstring, token, ' '))
        instructions.push_back(token);
    string keyword = instructions[0];
    switch(cmdMap[keyword]) {
        case HELP:  cmd_help();  break;
        case GET:   cmd_get();   break;
        case PUT:   cmd_put();   break;
  		case LS:    cmd_ls();    break;
        case PWD:   cmd_pwd();   break;
  		case OPEN:  cmd_open();  break;
      	case CLOSE: cmd_close(); break;
		case QUIT:  cmd_quit();  break;
        case CD:    cmd_cd();    break;
		default:    error_Msg("?Invalid command");
    }
}

void ftpClient::getPasv()
{
    // PASV: this command request the server-DTP to "listen" on a data port
    // and wait to for a connection rather than initiate one upon receipt of
    // a transfer command. The respond to this command includes the host and
    // port address this server is listening on.
    if (sendServerCmd("PASV") > 0) {
        if (getReplyFromServer() > 0) {
            //cout << "getPasv(): replymsg: " << getReplyMessage();
            replyCode = getReplyCode();
            //cout << "getPasv() replycode: " << replyCode << endl;
        }
    }
    if (replyCode == 227) {
        int port = getPort(replymsg);
        //cout << "getPasv() port: " << port << endl;
        if ((pasvSock = openPasvSock(port)) > 0) {
            //cout << "openPasvSock() OK! pasvSock: " << pasvSock << endl;
            isPasv = true;
            return ;
        } else {
            isPasv = false;
            return ;
        }
    }
    isPasv = false;
}

bool ftpClient::pasvReady()
{
    if (isPasv) {
        cout << "200 PORT command successful." << endl;
        return true;
    } else {
        cout << "Passive mode is not ready." << endl;
        return false;
    }
}

int ftpClient::openPasvSock(int _port)
{
    //cout << "openPasvSock(): ip: " << ip << " port: " << _port << endl;
    return Connect(ip, _port);
    /*
    struct hostent* host = gethostbyname(ip.c_str());
    sockaddr_in pasvAddr;
    bzero((void*) &pasvAddr, sizeof(pasvAddr));
    pasvAddr.sin_family = AF_INET;
    pasvAddr.sin_port = htons(_port);
    pasvAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    //inet_pton(AF_INET, ip.c_str(), &pasvAddr.sin_addr);
    pasvSock = socket(AF_INET, SOCK_STREAM, 0);
    if (pasvSock == -1) error_Exit("socket() error!");
    cout << "pasvSock: " << pasvSock << endl;
    cout << "clntSock: " << clntSock << endl;
    if (connect(pasvSock, (struct sockaddr*) &pasvAddr, sizeof(pasvAddr)) == -1)
        error_Exit("connect() error!");
    */
    //return pasvSock;
}

// 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2).
// PASV port = 256 * p1 + p2
int ftpClient::getPort(const string& msg)
{
    string token;
    std::stringstream str(msg);
    vector<string> vs;
    while (getline(str, token, ','))
        vs.push_back(token);
    int port1 = 0;
    port1 = std::stoi(vs[vs.size()-2]); // need compile -std=c++11
    char* p2 = (char*)(vs[vs.size()-1]).c_str();
    int port2 = 0;
    port2 = atoi(strtok(p2, ")"));
    //cout << "getPort(): port1: " << port1 << " port2: " << port2 << endl;
    return 256 * port1 + port2;
}

void ftpClient::cmd_help()
{
    if (instructions.size() == 1) {
        std::cout << "Command may be abbreviated.  Command are:" << std::endl;
        std::cout << "";
    }
}

void ftpClient::cmd_get()
{

}

void ftpClient::cmd_put()
{

}

int ftpClient::getAsciiMsgFromServer()
{
    struct pollfd ufds;
    //cout << "getAsciiMsgFromServer(): pasvSock: " << pasvSock << endl;
    ufds.fd = pasvSock;
    ufds.events = POLLIN; // There is data to read
    ufds.revents = 0;
    asciiMsg.clear();

    char buf[1024];
    int len = 0;
    int total = 0;
    while (poll(&ufds, 1, 1000) > 0) {
        bzero(buf, sizeof(buf));
        len = read(pasvSock, (char*)&buf, sizeof(buf));
        if (len == 0) break;
        asciiMsg.append(buf);
        total += len;
    }
    return total;
}

string ftpClient::getAsciiMsg()
{
    return asciiMsg;
}

void ftpClient::cmd_ls()
{
    getPasv();
    if (!pasvReady())
        return ;
    int status;
    int pid;
    //cout << "pacvSock: " << pasvSock << endl;
    if ((pid = fork()) < 0) {
        error_Exit("fork() error!");
    } else if (pid == 0) {
        if (getAsciiMsgFromServer() > 0) {
            cout << getAsciiMsg();
        }
        shutdown(pasvSock, SHUT_WR);
        close(pasvSock);
        exit(0);
    } else if (pid > 0) {
        if (sendServerCmd("LIST\r\n") > 0) {
            if (getReplyFromServer() > 0) {
                cout << getReplyMessage();
            }
            wait(&status);
            if (getReplyFromServer() > 0) {
                cout << getReplyMessage();
            }
        }
    }
}

void ftpClient::cmd_pwd()
{

}

void ftpClient::cmd_open()
{

}

void ftpClient::cmd_close()
{

}

void ftpClient::cmd_quit()
{

}

void ftpClient::cmd_cd()
{

}


