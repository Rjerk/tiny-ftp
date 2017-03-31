#include "ftpClient.h"

enum cmdType {
    HELP = 500,
    GET,
    PUT,
    LS,
    OPEN,
    CLOSE,
    QUIT,
    CD,
    PWD,
    DELETE,
    SYST
};

map<string, cmdType> cmdMap;

ftpClient::ftpClient(string _ip, int _port):
    ip(_ip), port(_port), running(true), connected(false), clntSock(-1)
{
    init();
}

ftpClient::~ftpClient()
{
}

void ftpClient::init()
{
	cmdMap["help"] = HELP;
    cmdMap["get"] = GET;
    cmdMap["put"] = PUT;
    cmdMap["ls"] = LS;
    cmdMap["open"] = OPEN;
    cmdMap["close"] = CLOSE;
    cmdMap["quit"] = QUIT;
    cmdMap["cd"] = CD;
    cmdMap["pwd"] = PWD;
    cmdMap["delete"] = DELETE;
    cmdMap["system"] = SYST;
}

void ftpClient::runClient()
{
    clntSock = Connect(ip, port);
    cout << "Connected to " << ip << "." << endl;
    pi.Configure(ip, clntSock);

    int replyCode = -1;
    if (pi.getReplyFromServer() > 0) {
        // 220 (vsFTPd 3.0.3)
        cout << pi.getReplyMessage();
        replyCode = pi.getReplyCode();
    }

    if (replyCode == 220) {
    	cout << "Name (" << ip << ":" << getUserName() << "): ";
        if (pi.loginServer()) {
            cout << "Remote system type is UNIX." << endl;
            cout << "Using binary mode to transfer files." << endl;
            connected = true;
        } else {
            cout << "Login failed." << endl;
        }
    }
}

void ftpClient::runPI()
{
	do {
        cout << "ftp> ";
        getline(cin, cmd);
    } while (cmd.empty());

    vector<string>().swap(instructions);

    std::stringstream cmdstring(cmd);
    string token;
    while (getline(cmdstring, token, ' '))
        instructions.push_back(token);
    string keyword = instructions[0];

	switch(cmdMap[keyword]) {
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

void ftpClient::cmd_help()
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

void ftpClient::cmd_delete()
{
    if (!isConnected()) {
        error_Msg("Not connected.");
        return ;
    }
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

void ftpClient::cmd_system()
{
    if (!isConnected()) {
        error_Msg("Not connected.");
        return ;
    }
    if (pi.sendServerCmd("SYST") > 0) {
        if (pi.getReplyFromServer() > 0) {
            cout << pi.getReplyMessage();
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

void ftpClient::cmd_put()
{
	if (!isConnected()) {
        error_Msg("Not connected.");
        return ;
    }
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

void ftpClient::cmd_ls()
{
	if (!isConnected()) {
        error_Msg("Not connected.");
        return ;
    }
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

void ftpClient::cmd_close()
{
	if (!isConnected()) {
        error_Msg("Not connected.");
        return ;
    }
    if (pi.sendServerCmd("QUIT") > 0) {
        if (pi.getReplyFromServer() > 0) {
            // 221 Goodbye
            cout << pi.getReplyMessage();
        }
    }
    Disconnect();
}

void ftpClient::cmd_pwd()
{
	if (!isConnected()) {
        error_Msg("Not connected.");
        return ;
    }
    //int replyCode;
    if (pi.sendServerCmd("PWD") > 0) {
        if (pi.getReplyFromServer() > 0) {
            cout << pi.getReplyMessage();
            //replyCode = pi.getReplyCode();
        }
    }
    // replyCode == 257?
}

void ftpClient::cmd_open()
{
    if (isConnected()) {
        error_Msg("Already connected to " + ip + ", use close first.");
        return ;
    }
    if (instructions.size() == 2) {
        ip = instructions[1];
    } else {
        cout << "(to) ";
        getline(cin, ip);
    }
    runClient();
}

void ftpClient::cmd_quit()
{
    if (!isConnected()) {
        cmd_close();
        return ;
    } else {
        exit(0);
    }
}

void ftpClient::cmd_cd()
{
    if (!isConnected()) {
        error_Msg("Not connected.");
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
        error_Msg("usage: cd remote-directory");
        return ;
    }

    //int replyCode;
    // CWD: This command allows the user to work with a different
    // directory or dataset for file storage or retrieval without
    // altering his login or accounting information.  Transfer
    // parameters are similarly unchanged.  The argument is a
    // pathname specifying a directory or other system dependent
    // file group designator.
    // CWD  <SP> <pathname> <CRLF>
    if (pi.sendServerCmd("CWD " + directory) > 0) {
        if (pi.getReplyFromServer() > 0) {
            cout << pi.getReplyMessage();
            //replyCode = pi.getReplyCode();
        }
    }
}

bool ftpClient::isRunning()
{
	return running;
}

void ftpClient::Disconnect()
{
    connected = false;
	close(clntSock);
}

bool ftpClient::isConnected()
{
	return connected;
}
