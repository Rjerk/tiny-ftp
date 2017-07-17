#include "FtpClient.h"
#include <cstdlib>

namespace ftpclient {

FtpClient::FtpClient(const std::string& ip, int16_t port)
        : handler(ip, port), ip_(ip), port_(port)
{

}

FtpClient::~FtpClient()
{

}

void FtpClient::connectToServer()
{
    handler.buildControlConnection();
    handler.buildDataTrassfer();
	handler.runShell();
}

void FtpClient::run(int argc, char* argv[])
{
	if (argc == 1) { // 进入 shell 使用控制连接登录服务器 
		handler.runShell();
	} else if (argc == 2) { // 使用默认的 port 21 连接服务器 
		FtpClient(string(argv[1]));
		connectToServer();
	} else if (argc >= 3) { // 指定 port 连接服务器 
		FtpClient(string(argv[1]), atoi(argv[2]));
		connectToServer();
	}
}

}

