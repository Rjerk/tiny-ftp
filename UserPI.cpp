#include "UserPI.h"

namespace ftpclient {

UserPI::UserPI(const string& ip_, int16_t port_)
	: stream(nullptr), ip(ip_), port(port_), reply_code(-1), pasv_sock(-1),
      cmd_sock(-1), is_pasv(false), reply_msg(""), ascii_msg("")
{
}

UserPI::~UserPI()
{
}

void UserPI::connect()
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
    cmd_sock = stream->getSock();
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
            assert(reply_code == 331);
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
                assert(getReplyCode() == 230);
                return getReplyCode() == 230;
            }
        }
    }
    return false;
}

int UserPI::getReplyFromServer()
{
	reply_msg.clear();
    char buf[1024];
    bzero(buf, sizeof(buf));

    int len = stream->receiveSome(buf, sizeof(buf));
	if (len == -1) {
        error_Exit("read() error");
    } else if (len > 0) {
        reply_msg = string(buf);
        reply_code = (reply_msg.length() >= 3 ? std::stoi(reply_msg.substr(0, 3)) : 0);
    }
    return len;
}

int UserPI::sendServerCmd(const string& cmd)
{
	int len = cmd.length() + 2;
    char buf[len];
    bzero(buf, sizeof(buf));
    strncpy(buf, cmd.c_str(), len);
    strcat(buf, "\r\n");
    return stream->sendAll(buf, len);
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

void UserPI::closeConn()
{
	stream->closeConn();
}

}
