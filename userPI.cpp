#include "userPI.h"
#include "error.h"
#include "utility.h"

int userPI::getReplyFromServer() //ok
{
	replymsg.clear();
    char buf[1024];
    bzero(buf, sizeof(buf));
    int len = read(clntSock, buf, sizeof(buf));
	if (len == -1) {
        error_Exit("read() in getReplyFromServer() error!");
    } else if (len > 0) {
        replymsg = string(buf);
    }
    return len;
}

string userPI::getReplyMessage() //ok
{
	return replymsg;
}

int userPI::getReplyCode() //ok
{
	if (replymsg.length() > 0) {
        string msg = replymsg;
        char* code = strtok((char*)msg.c_str(), " ");
        replyCode = atoi(code);
        return replyCode;
    }
    return 0;
}

bool userPI::loginServer() // ok
{
    string username;
    string password;
    getline(cin, username);
    if (username.size() == 0) username = " ";
    if (sendServerCmd("USER " + username) > 0) {
        if (getReplyFromServer() > 0) {
            // reply: 331 Please specify the password.
            cout << getReplyMessage();
            replyCode = getReplyCode();
        }
    }
    if (replyCode == 331) {
        cout << "Password: ";
		hideInput();
        getline(cin, password);
		showInput();
		cout << "\n";
        string passwd = "PASS " + password;
        if (sendServerCmd(passwd) > 0) {
            if (getReplyFromServer() > 0) {
                // reply: 230 Login Successful.
                cout << getReplyMessage();
                replyCode = getReplyCode();
                if (replyCode == 230)
                    return true;
            }
        }
    }
    return false;
}

void userPI::getPasv()
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
        }
    }
    isPasv = false;
}

bool userPI::pasvReady()
{
    if (isPasv) {
        cout << "200 PORT command successful." << endl;
        return true;
    } else {
        cout << "Passive mode is not ready." << endl;
        return false;
    }
}

int userPI::openPasvSock(int _port)
{
    return Connect(ip, _port);
}

// 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2).
// PASV port = 256 * p1 + p2
int userPI::getPort(const string& msg)
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

int userPI::getAsciiMsgFromServer()
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

int userPI::receiveFile(int fileSock)
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

int userPI::sendFile(int fileSock)
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

int userPI::sendServerCmd(const string& cmd)
{
	int len = cmd.length() + 2;
    char buf[len];
    bzero(buf, sizeof(buf));
    strcpy(buf, cmd.c_str());
    strcat(buf, "\r\n");
    return write(clntSock, buf, len);
}

string userPI::getAsciiMsg()
{
    return asciiMsg;
}

void userPI::Configure(string _ip, int _clntSock)
{
	ip = _ip;
	clntSock = _clntSock;
}
