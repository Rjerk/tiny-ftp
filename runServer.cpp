#include "FtpServer.h"
#include "error.h"

#include <cstdlib>

using namespace ftp;
using namespace ftpserver;

int main(int argc, char* argv[])
{
	if (argc != 2) {
		error_Msg("usage: ./ftpserver port");
		return -1;
	}

	FtpServer server(atoi(argv[1]));
	while (true) {
		server.run();
	}
}
