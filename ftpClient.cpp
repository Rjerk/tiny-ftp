#include "ftpClient.h"
#include <string>

enum cmdType {
    HELP, GET, PUT, LS, OPEN, CLOSE, QUIT, CD, PWD
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
    cmdMap["help"] = HELP;
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
    return len;
}

void ftpClient::runClient()
{
    clntSock = Connect(ip, port);
    cout << "Connected to " << ip << "." << endl;

    if (getReplyFromServer() > 0) {
        // 220 (vsFTPd 3.0.3)
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
    cout << "ftp> ";
    getline(cin, cmd);
    vector<string>().swap(instructions);
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
            replyCode = getReplyCode();
        }
    }
    if (replyCode == 227) {
        int port = getPort(replymsg);
        if ((pasvSock = openPasvSock(port)) > 0) {
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
    return Connect(ip, _port);
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
    return 256 * port1 + port2;
}

void ftpClient::cmd_help()
{
    if (instructions.size() == 1) {
        std::cout << "Commands may be abbreviated.  Commands are:" << std::endl;
        std::cout << "cd\t\tclose\t\tget\t\thelp\t\tls\nopen\t\tput\t\tpwd\t\tquit\n";
    } else if (instructions.size() == 2) {
		switch(cmdMap[instructions[1]]) {
		    case CD:
		        cout << "cd\t\tchange remote working directory" << endl;
		        break;
		    case CLOSE:
		        cout << "close\t\tterminate ftp session" << endl;
		        break;
		    case GET:
		        cout << "get\t\treceive file" << endl;
		        break;
		    case HELP:
		        cout << "help\t\tprint local help information" << endl;
		        break;
		    case LS:
		        cout << "ls\t\tlist contents of remote directory" << endl;
		        break;
		    case OPEN:
		        cout << "open\t\tconnect to remote ftp" << endl;
		        break;
		    case PUT:
		        cout << "put\t\tsend one file" << endl;
		        break;
		    case PWD:
		        cout << "pwd\t\tprint working directory on remote machine" << endl;
		        break;
		    case QUIT:
		        cout << "quit\t\tterminate ftp session and exit" << endl;
		        break;
		    default:
		        error_Msg("?Invalid help command "+instructions[1]);
		}
    }
}

void ftpClient::cmd_get()
{
    if (!isConnected()) {
        error_Msg("Not connected.");
        return ;
    }
    string remotefile;
    string localfile;

    if (instructions.size() == 1) {
        cout << "(remote-file) ";
        getline(cin, remotefile);
        cout << "(local-file) ";
        getline(cin, localfile);
    } else if (instructions.size() == 2) {
        remotefile = instructions[1];
        localfile = instructions[1];
    } else if (instructions.size() == 3) {
        remotefile = instructions[1];
        localfile = instructions[2];
    }
    if (remotefile.size() == 0) {
        error_Msg("usage: get remote-file [ local-file ]");
        return ;
    }
    cout << "local: " << localfile << " remote: " << remotefile << endl;
    if (sendServerCmd("TYPE I") > 0) {
        if (getReplyFromServer() > 0) {
            replyCode = getReplyCode();
        }
    }
    ofstream outfile(localfile, ofstream::out); // need complie -std=c++11
    if (!outfile.good()) {
        error_Msg("local: " + localfile + ": Permission denied");
        return ;
    }

    getPasv();
    if (!pasvReady())
        return ;

    if (sendServerCmd("RETR " + remotefile) > 0) {
        if (getReplyFromServer() > 0) {
            // 150 Opening BINARY mode data connection for <remotefile> (xx bytes).
            cout << getReplyMessage();
            replyCode = getReplyCode();
        }

    if (replyCode == 150) {
        int status;
        int pid = fork();
        if (pid < 0) {
            error_Exit("fork() error!");
        } else if (pid == 0) {
            struct timeval start, end;
            gettimeofday(&start, NULL);
            int s = getAsciiMsgFromServer();
            gettimeofday(&end, NULL);
            if (getReplyFromServer() > 0) {
                // 226 Transfer Complete
                cout << getReplyMessage();
                replyCode = getReplyCode();
            }
			double transTime = ((end.tv_usec - start.tv_usec) + 1000000 * (end.tv_sec - start.tv_sec)) / 1000000.0;
            if (replyCode == 226) {
                cout << s << " bytes received in " << std::setprecision(5) << transTime << " secs ("
                     << std::setprecision(5) << s / transTime / 1024 << " kB/s)" << endl;
            }
            exit(0);
        } else if (pid > 0) {
            wait(&status);
        }
    }
}

void ftpClient::cmd_put()
{

}

int ftpClient::getAsciiMsgFromServer()
{
    struct pollfd ufds;
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
    if ((pid = fork()) < 0) {
        error_Exit("fork() error!");
    } else if (pid == 0) {
        if (getAsciiMsgFromServer() > 0) {
            cout << getAsciiMsg();
        }
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
    if (!isConnected()) {
        error_Msg("Not connected.");
        return ;
    }
    if (sendServerCmd("PWD") > 0) {
        if (getReplyFromServer() > 0) {
            cout << getReplyMessage();
            replyCode = getReplyCode();
        }
    }
    // replyCode == 257?
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
    if (!isConnected()) {
        error_Exit("Not connected.");
        return ;
    }
    string directory;
    if (instructions.size() == 1) {
        cout << "(remote-directory) ";
        getline(cin, directory);
    } else {
        directory = instructions[1];
    }

    if (directory.size() == 0) {
        error_Exit("usage: cd remote-directory");
        return ;
    }

    // CWD: This command allows the user to work with a different
    // directory or dataset for file storage or retrieval without
    // altering his login or accounting information.  Transfer
    // parameters are similarly unchanged.  The argument is a
    // pathname specifying a directory or other system dependent
    // file group designator.
    // CWD  <SP> <pathname> <CRLF>
    if (sendServerCmd("CWD " + directory + "\r\n") > 0) {
        if (getReplyFromServer() > 0) {
            cout << getReplyMessage();
            replyCode = getReplyCode();
        }
    }

}


