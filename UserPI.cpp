#include "UserPI.h"

namespace ftpclient {

const int CONTROL_PORT = 21;
const int DATATRANS_PORT = 20;

UserPI::UserPI()
	: is_pasv(false) 
{

}

UserPI::~UserPI()
{

}

void UserPI::connect(const string& ip, int16_t port)
{
    InetAddr addr(port);
    if (!InetAddr::resolve(ip, &addr)) {
        error_Exit("Unable to resolve" + ip);
        return ;
    }

    stream = TcpStream::connect(addr);
    if (!stream) {
        error_Exit("Unable to connect " + addr.toIpPort());
        return ;
    }
}

int UserPI::getReplyFromServer()
{
	reply_msg.clear();
    char buf[1024];
    bzero(buf, sizeof(buf));
    // FIXME: EINTR
    int len = read(CONTROL_PORT, buf, sizeof(buf));
	if (len == -1) {
        error_Exit("read() in getReplyFromServer() error!");
    } else if (len > 0) {
        reply_msg = string(buf);
        reply_code = (reply_msg.length() >= 3 ? std::stoi(reply_msg.substr(0, 3)) : 0);
    }
    return len;
}

bool UserPI::loginServer()
{
    string username;
    string password;

    getline(cin, username);

    if (username.size() == 0)
		username = " ";

    if (sendServerCmd("USER " + username) > 0) {
        if (getReplyFromServer() > 0) { // reply: 331 Please specify the password.
            cout << getReplyMessage();
        }
    }

    if (getReplyCode() == 331) {
        cout << "Password: ";
		hideInput();
        getline(cin, password);
		showInput();
		cout << "\n";
        string passwd = "PASS " + password;
        if (sendServerCmd("PASS " + password) > 0) {
            if (getReplyFromServer() > 0) { // reply: 230 Login Successful.
                cout << getReplyMessage();
                return getReplyCode() == 230;
            }
        }
    }
    return false;
}

int UserPI::sendServerCmd(const string& cmd)
{
	int len = cmd.length() + 2;
    char buf[len];
    bzero(buf, sizeof(buf));
    strcpy(buf, cmd.c_str());
    strcat(buf, "\r\n");
    return write(clntSock, buf, len);
}

// 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2).
// PASV port = 256 * p1 + p2
int UserPI::getPasvPortFromReply(const string& msg)
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

int UserPI::openPasvSock(int _port)
{
    return Connect(ip, _port);
}

string UserPI::getAsciiMsg()
{
    return asciiMsg;
}

void UserPI::Configure(string _ip, int _clntSock)
{
	ip = _ip;
	clntSock = _clntSock;
}

bool UserPI::pasvReady()
{
    if (isPasv) {
        cout << "200 PORT command successful." << endl;
        return true;
    } else {
        cout << "Passive mode is not ready." << endl;
        return false;
    }
}

int UserPI::getAsciiMsgFromServer()
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
    close(pasvSock);
    return total;
}

int UserPI::receiveFile(int fileSock)
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
        if ((len = read(pasvSock, (char*)&buf, sizeof(buf))) == -1) {
        	error_Exit("read() error in receiveFile().");
		}
        if (len == 0) break;
        lseek(fileSock, 0, SEEK_END);
        if (write(fileSock, buf, len) == -1) {
        	error_Exit("write() error in receiveFile().");
    		return -1;
		}
        total += len;
    }
    close(pasvSock);
	close(fileSock);
    return total;
}

int UserPI::sendFile(int fileSock)
{
	int sz = lseek(fileSock, 0, SEEK_END);
    lseek(fileSock, 0, 0);
    char* buf = new char[sz];
    if (read(fileSock, buf, sz) == -1) {
    	error_Exit("read() error in sendFile().");
	}
    close(fileSock);
    int len;
    if ((len = write(pasvSock, buf, sz)) == -1) {
    	error_Exit("write() error in sendFile().");
	}
    delete [] buf;
    close(pasvSock);
    return len;
}

void UserPI::closeConn()
{
	
}

}
