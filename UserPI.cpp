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
    return write(clnt_sock, buf, len);
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
    char* p2 = const_cast<char*>((vs[vs.size()-1]).c_str());
    int port2 = 0;
    port2 = atoi(strtok(p2, ")"));
    return 256 * port1 + port2;
}

string UserPI::getAsciiMsg()
{
    return ascii_msg;
}

void UserPI::Configure(string ip_, int clnt_sock_)
{
	ip = ip_;
	clnt_sock = clnt_sock_;
}

bool UserPI::isPasvReady()
{
    if (is_pasv) {
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
    ufds.fd = pasv_sock;
    ufds.events = POLLIN; // There is data to read
    ufds.revents = 0;
    ascii_msg.clear();

    char buf[1024];
    int len = 0;
    int total = 0;
    while (poll(&ufds, 1, 1000) > 0) {
        bzero(buf, sizeof(buf));
        len = read(pasv_sock, static_cast<void*>(&buf), sizeof(buf));
        if (len == 0) break;
        ascii_msg.append(buf);
        total += len;
    }
    close(pasv_sock);
    return total;
}

int UserPI::receiveFile(int fd)
{
	struct pollfd ufds;
    ufds.fd = pasv_sock;
    ufds.events = POLLIN; // There is data to read
    ufds.revents = 0;
    ascii_msg.clear();

    char buf[1024];
    int len = 0;
    int total = 0;
    while (poll(&ufds, 1, 1000) > 0) {
        bzero(buf, sizeof(buf));
        if ((len = read(pasv_sock, static_cast<void*>(&buf), sizeof(buf))) == -1) {
        	error_Exit("read() error in receiveFile().");
		}
        if (len == 0) break;
        lseek(fd, 0, SEEK_END);
        if (write(fd, buf, len) == -1) {
        	error_Exit("write() error in receiveFile().");
    		return -1;
		}
        total += len;
    }
    close(pasv_sock);
	close(fd);
    return total;
}

int UserPI::sendFile(int file_sock)
{
	int sz = lseek(file_sock, 0, SEEK_END);
    lseek(file_sock, 0, 0);
    char* buf = new char[sz];
    if (read(file_sock, buf, sz) == -1) {
    	error_Exit("read() error in sendFile().");
	}
    close(file_sock);
    int len;
    if ((len = write(pasv_sock, buf, sz)) == -1) {
    	error_Exit("write() error in sendFile().");
	}
    delete [] buf;
    close(pasv_sock);
    return len;
}

void UserPI::closeConn()
{
	stream->closeConn();
}

}
