#include "FtpClient.h"

using namespace ftpclient;

int main(int argc, char* argv[])
{
	FtpClient* client = nullptr;
	client->run(argc, argv);
}
