#include "ClientConn.h"
#include <utility>
#include <unistd.h>
#include <iostream>
#include <unordered_map>

using namespace ftp;
using namespace ftpserver;

using std::cout;

namespace {

const string C220 = "220 (rftp 0.1).\r\n";
const string C331 = "331 Please specify the password.\r\n";
const string C230 = "230 Login Successful.\r\n";
const string C530 = "530 Permission Denied.\r\n"; // user name is incorrect.
const string C331_2 = "331 Login incorrect.\r\n"; // passwd is incorrect.
const string C227 = "227 Entering Passive Mode (127,0,0,1,190,217).\r\n";
//const string C150 = "150 Here comes the directory listing.\r\n";
//const string C226 = "226 Directory send ok.\r\n";
//const string C250 = "250 Directory successfully changed.\r\n";
//const string C250_2 = "250 Delete operation successful.\r\n";
//const string C215 = "215 Unix Type: L8\r\n";
//const string C257 = "257 "\/upload" is the current directory\r\n";
const string C221 = "221 Goodbye.\r\n";
//const string C200 = "200 Switching to Binary mode.\r\n"; // get c.cpp
//const string C150 = "150 Opening BINARY mode data connection for c.cpp (186 bytes).\r\n";
//const string C226_2 = "226 Transfer complete.\r\n";
//const string C150_2 = "150 Ok to send data.\r\n"; // put main.cpp
//const string C331 = "226 Transfer complete.\r\n";

enum SERVER_CMD {
	USER, PASS, PASV, DELE, SYST, TYPE,
	RETR, STOR, LIST, QUIT, PWD,  CWD
};

std::unordered_map<string, SERVER_CMD> server_cmd_map = {
	{ "USER", USER }, { "PASS", PASS } , { "PASV", PASS },
	{ "DELE", DELE }, { "SYST", SYST } , { "TYPE", TYPE },
	{ "RETR", RETR }, { "STOR", STOR } , { "LIST", LIST },
	{ "QUIT", QUIT }, { "PWD ", PWD  } , { "CWD ", CWD },
};

}

ClientConn::ClientConn(Socket&& conn_sock_)
	: conn_sock(std::move(conn_sock_)), is_connected(true)
{
}

ClientConn::~ClientConn()
{
	conn_sock.closeConn();
}

void ClientConn::handleRequest()
{
	conn_sock.write(C220.c_str(), C220.size());
	char buf[1024];
	while (is_connected) {
		bzero(&buf, sizeof(buf));
		conn_sock.read(buf, sizeof(buf));
		string request = string(buf);
		cout << request << "\n";
		if (request.size() < 4)
			continue;
		switch(server_cmd_map[request.substr(0, 4)]) {
			case USER:
				handleUserCmd(request);
				break;
			case PASS:
				handlePassCmd(request);
				break;
			case PASV:
				handlePasvCmd(request);
				break;
			case DELE:
				handleDeleCmd(request);
				break;
			case SYST:
				handleSystCmd(request);
				break;
			case TYPE:
				handleTypeCmd(request);
				break;
			case RETR:
				handleRetrCmd(request);
				break;
			case STOR:
				handleStorCmd(request);
				break;
			case LIST:
				handleListCmd(request);
				break;
			case QUIT:
				handleQuitCmd(request);
				break;
			case PWD:
				handlePwdCmd(request);
				break;
			case CWD:
				handleCwdCmd(request);
				break;
			default:
				break;
		}
	}
}

void ClientConn::handleUserCmd(const string& request)
{
	// TODO: get username list.
	if( request.substr(5).substr(0, request.substr(5).size()-2) == "sam" ) {
		conn_sock.write(C331.c_str(), C331.size());
	} else {
		conn_sock.write(C530.c_str(), C530.size());
		conn_sock.closeConn();
		is_connected = false;
	}
}

void ClientConn::handlePassCmd(const string& request)
{
	if ( request.substr(5).substr(0, request.substr(5).size()-2) == "123123" ) {
		conn_sock.write(C230.c_str(), C230.size());
	} else {
		conn_sock.write(C331_2.c_str(), C331_2.size());
		conn_sock.closeConn();
		is_connected = false;
	}
}

void ClientConn::handlePasvCmd(const string&)
{
}

void ClientConn::handleDeleCmd(const string& request)
{
	(void) request;
}

void ClientConn::handleSystCmd(const string&)
{
}

void ClientConn::handleTypeCmd(const string& request)
{
	(void) request;
}

void ClientConn::handleRetrCmd(const string& request)
{
	(void) request;
}

void ClientConn::handleStorCmd(const string& request)
{
	(void) request;
}

void ClientConn::handleListCmd(const string&)
{
}

void ClientConn::handleQuitCmd(const string&)
{
	conn_sock.write(C221.c_str(), C221.size());
	is_connected = false;
}

void ClientConn::handlePwdCmd(const string&)
{
}

void ClientConn::handleCwdCmd(const string& request)
{
	(void) request;
}
