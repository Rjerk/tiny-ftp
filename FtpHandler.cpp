#include "FtpHandler.h"
#include "InetAddr.h"
#include "utility.h"

namespace ftpclient {

FtpHandler::FtpHandler(std::string ip, int16_t port)
    : up(), ud(), ip_(ip), port_(port),
	is_connected(false), is_running(true), cmd(""), instructions()
{
}

void FtpHandler::connect()
{
    up.connect(ip_, port_); // open a socket and connect to ftp server.
}

void FtpHandler::login()
{
	int reply_code = -1;
	if (up.getReplyFromServer() > 0) { // 220 (vsFTPd 3.0.3)
		cout << up.getReplyMessage();
	}

	if (up.getReplyCode() == 220) {
		cout << "Name (" << ip << ":" << getUserName() << "): ";
        if (up.loginServer()) {
            cout << "Remote system type is UNIX." << endl;
            cout << "Using binary mode to transfer files." << endl;
            is_connected = true;
            return true;
        } else {
            cout << "Login failed." << endl;
            return false;
        }
	}
	return false;
}

void FtpHandler::buildControlConnection()
{
	connect();
	login();
}

void FtpHandler::runShell()
{
	while (isRunning()) {

		do {
			cout << "ftp> ";
			getline(cin, cmd);
		} while (cmd.empty());

		vector<string>().swap(instructions);

	    std::stringstream cmdstring(cmd);
	    string token;
	    while (getline(cmdstring, token, ' ')) {
	    	instructions.push_back(token);
		}
	
		switch(cmdMap[instructions[0]]) {
			case HELP:   cmd_help();    break;
			case GET:    cmd_get();     break;
		    case PUT:    cmd_put();     break;
		    case DELETE: cmd_delete();  break;
		    case SYST:   cmd_system();  break;
		  	case LS:     cmd_ls();      break;
			case PWD:    cmd_pwd();     break;
		  	case OPEN:   cmd_open();    break;
		    case CLOSE:  cmd_close();   break;
			case QUIT:   cmd_quit();    break;
		    case CD:     cmd_cd();      break;
			default:     error_Msg("?Invalid command");
		}
		
	}
}

void FtpHandler::buildDataTrassfer()
{
	if (up.sendServerCmd("PASV") > 0) {
        if (up.getReplyFromServer() == 0) {
            is_pasv_conn = false;
            return ;
        }
    }

    if (replyCode == 227) {
        int port = up.getPort(replymsg); // 获得服务器被动打开的端口号 
        int pasv_sock;
        if ((pasv_sock = ud.openPasvSock(port)) >= 0) { // 客户端建立数据通道监听服务器的数据端口 
            is_pasv_conn = true;
            return ;
        }
    }
    is_pasv_conn = false;
}

void FtpHandler::cmd_help()
{
    if (instructions.size() == 1) {
        std::cout << "Commands may be abbreviated.  Commands are:" << std::endl;
        std::cout << "cd\t\tclose\t\tget\t\tdelete\t\tsystem\nhelp\t\tls\t\topen\t\tput\t\tpwd\nquit\n";
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
            case DELETE:
                cout << "delete\t\tdelete remote file" << endl;
                break;
            case SYST:
                cout << "system\t\tshow remote system type" << endl;
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

void FtpHandler::cmd_delete()
{
    if (!checkConnected())
		return ;
    string remotefile;
    if (instructions.size() == 1) {
        cout << "(remote-file) ";
        getline(cin, remotefile);
    } else if (instructions.size() == 2) {
        remotefile = instructions[1];
    }

    if (remotefile.size() == 0) {
        error_Msg("usage: delete remote-file");
        return ;
    }

    if (pi.sendServerCmd("DELE " + remotefile) > 0) {
        if (pi.getReplyFromServer() > 0) {
            cout << pi.getReplyMessage();
        }
    }
}

void FtpHandler::cmd_system()
{
    if (!checkConnected())
		return ;
    if (pi.sendServerCmd("SYST") > 0) {
        if (pi.getReplyFromServer() > 0) {
            cout << pi.getReplyMessage();
        }
    }
}

void FtpHandler::cmd_get()
{
	if (!checkConnected())
		return ;
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
    int replyCode = -1;
    if (pi.sendServerCmd("TYPE I") > 0) {
        if (pi.getReplyFromServer() > 0) {
            replyCode = pi.getReplyCode();
        }
    }

    string replymsg = pi.getReplyMessage();
    if (replyCode != 200) {
        error_Msg(replymsg);
        return ;
    }

    pi.getPasv();
    if (!pi.pasvReady())
        return ;

    if (pi.sendServerCmd("RETR " + remotefile) > 0) {
        if (pi.getReplyFromServer() > 0) {
            // 150 Opening BINARY mode data connection for <remotefile> (xx bytes).
            cout << pi.getReplyMessage();
            replyCode = pi.getReplyCode();
        }
    }
    if (replyCode == 150) {
        int fileSock;
        if ((fileSock = open(localfile.c_str(), O_WRONLY)) == -1) {
            error_Msg("local: " + localfile + ": Permission denied");
            return ;
        }
        struct timeval start, end;
        gettimeofday(&start, NULL);
        int s = pi.receiveFile(fileSock);
        gettimeofday(&end, NULL);
        if (pi.getReplyFromServer() > 0) {
        // 226 Transfer Complete
        cout << pi.getReplyMessage();
        replyCode = pi.getReplyCode();
        }
        double transTime = ((end.tv_usec - start.tv_usec)
            + 1000000 * (end.tv_sec - start.tv_sec)) / 1000000.0;
        if (replyCode == 226) {
            cout << s << " bytes received in "
                 << std::setprecision(5) << transTime
                 << " secs (" << std::setprecision(5)
                 << s / transTime / 1024 << " kB/s)" << endl;
        }
    }
}

void FtpHandler::cmd_put()
{
	if (!checkConnected())
		return ;
	string remotefile;
    string localfile;

    if (instructions.size() == 1) {
        cout << "(local-file) ";
        getline(cin, localfile);
        if (localfile.size() == 0) {
            error_Msg("usage: put local-file remote-file");
            return ;
        }
        cout << "(remote-file) ";
        getline(cin, remotefile);
        if (remotefile.size() == 0) {
            error_Msg("usage: put local-file remote-file");
            return ;
        }
    } else if (instructions.size() == 2) {
        localfile = instructions[1];
        remotefile = instructions[1];
    } else if (instructions.size() == 3) {
        localfile = instructions[1];
        remotefile = instructions[2];
    }

    cout << "local: " << localfile << " remote: " << remotefile << endl;

    int replyCode = -1;
    if (pi.sendServerCmd("TYPE I") > 0) {
        if (pi.getReplyFromServer() > 0) {
            replyCode = pi.getReplyCode();
        }
    }
    string replymsg = pi.getReplyMessage();
    if (replyCode != 200) {
        error_Msg(replymsg);
        return ;
    }

    pi.getPasv();
    if (!pi.pasvReady())
        return ;

    int fileSock;
	if ((fileSock = open(localfile.c_str(), O_RDONLY)) == -1) {
		error_Msg("local: " + localfile + ": No such file or directory");
		return ;
	}

    if (pi.sendServerCmd("STOR " + remotefile) > 0) {
        if (pi.getReplyFromServer() > 0) {
            cout << pi.getReplyMessage();
            replyCode = pi.getReplyCode();
        }
    }

    if (replyCode == 150) {
	    struct timeval start, end;
		gettimeofday(&start, NULL);
		int s = pi.sendFile(fileSock);
		gettimeofday(&end, NULL);
		if (pi.getReplyFromServer() > 0) {
			// 226 Transfer Complete
			cout << pi.getReplyMessage();
			replyCode = pi.getReplyCode();
		}
		double transTime = ((end.tv_usec - start.tv_usec)
			+ 1000000 * (end.tv_sec - start.tv_sec)) / 1000000.0;
		if (replyCode == 226) {
		    cout << s << " bytes received in " << std::setprecision(5)
				 << transTime << " secs ("
		         << std::setprecision(5)
				 << s / transTime / 1024 << " kB/s)" << endl;
		}
	}
}

void FtpHandler::cmd_ls()
{
	if (!checkConnected())
		return ;
    pi.getPasv();
    if (!pi.pasvReady())
        return ;
    int status;
    int pid;
    if ((pid = fork()) < 0) {
        error_Exit("fork() error!");
    } else if (pid == 0) {
        if (pi.getAsciiMsgFromServer() > 0) {
            cout << pi.getAsciiMsg();
        }
        exit(0);
    } else if (pid > 0) {
        if (pi.sendServerCmd("LIST") > 0) {
            if (pi.getReplyFromServer() > 0) {
                cout << pi.getReplyMessage();
            }
            wait(&status);
            if (pi.getReplyFromServer() > 0) {
                cout << pi.getReplyMessage();
            }
        }
    }
}

void FtpHandler::cmd_close()
{
	if (!checkConnected())
		return ;
    if (pi.sendServerCmd("QUIT") > 0) {
        if (pi.getReplyFromServer() > 0) {
            // 221 Goodbye
            cout << pi.getReplyMessage();
        }
    }
    Disconnect();
}

void FtpHandler::cmd_pwd()
{
	if (!checkConnected())
		return ;

    if (pi.sendServerCmd("PWD") > 0) {
        if (pi.getReplyFromServer() > 0) {
            cout << pi.getReplyMessage();
            assert(reply_code == 257);
        }
    }
}

void FtpHandler::cmd_open()
{
    if (!checkConnected())
		return ;

	if (instructions.size() == 1) {
        cout << "(to) ";
        getline(cin, ip_);
	} else if (instructions.size() == 2) {
        ip_ = instructions[1];
    } else if (instructions.size() == 3) {
    	ip_ = instructions[1];
    	port_ = std::stoi(instructions[2]);
    } else {
    	error_Msg("usage: open host-name [port]");
	}
	
    buildControlConnection();
}

void FtpHandler::cmd_quit()
{
    if (!isConnected()) {
        cmd_close();
        return ;
    } else {
        exit(0);
    }
}

void FtpHandler::cmd_cd()
{
    if (!checkConnected())
		return ;

    string directory;
    if (instructions.size() == 1) {
        cout << "(remote-directory) ";
        getline(cin, directory);
    } else {
        directory = instructions[1];
    }

    if (directory.size() == 0) {
        error_Msg("usage: cd remote-directory");
        return ;
    }

    if (pi.sendServerCmd("CWD " + directory) > 0) {
        if (pi.getReplyFromServer() > 0) {
            cout << pi.getReplyMessage();
        }
    }
}

bool FtpHandler::checkConnected()
{
   	if (!isConnected()) {
    	error_Msg("Not connected.");
    	return false;
    }
   	return true;
}

}
