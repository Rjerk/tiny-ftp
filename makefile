objects = runClient.o FtpHandler.o UserDTP.o UserPI.o \
		  TcpStream.o InetAddr.o Socket.o  error.o utility.o
serverobj = runServer.o FtpServer.o ClientConn.o Acceptor.o \
			TcpStream.o InetAddr.o Socket.o error.o utility.o

flags = -std=c++11 -O -Wall -Wextra -Werror -Wold-style-cast -pthread

all: client server

client: $(objects)
	g++ $(flags) -o ftp $(objects)

server: $(serverobj)
	g++ $(flags) -o ftpserver $(serverobj)

runServer.o: runServer.cpp
	g++ $(flags) -c runServer.cpp

FtpServer.o: FtpServer.cpp
	g++ $(flags) -c FtpServer.cpp

ClientConn.o: ClientConn.cpp
	g++ $(flags) -c ClientConn.cpp

runClient.o: runClient.cpp
	g++ $(flags) -c runClient.cpp

FtpHandler.o: FtpHandler.cpp
	g++ $(flags) -c FtpHandler.cpp

UserDTP.o: UserDTP.cpp
	g++ $(flags) -c UserDTP.cpp

UserPI.o: UserPI.cpp 
	g++ $(flags) -c UserPI.cpp

TcpStream.o: TcpStream.cpp
	g++ $(flags) -c TcpStream.cpp

Acceptor.o: Acceptor.cpp
	g++ $(flags) -c Acceptor.cpp

Socket.o: Socket.cpp
	g++ $(flags) -c Socket.cpp

InetAddr.o: InetAddr.cpp
	g++ $(flags) -c InetAddr.cpp

utility.o: utility.cpp
	g++ $(flags) -c utility.cpp

error.o: error.cpp
	g++ $(flags) -c error.cpp

.PHONY: clean

clean:
	rm -f ftp ftpserver $(objects) $(serverobj)
