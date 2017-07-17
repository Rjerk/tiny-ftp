objects = main.o FtpHandler.o UserDTP.o UserPI.o \
		  TcpStream.o InetAddr.o Socket.o  error.o utility.o

flags = -std=c++11 -O -Wall -Wextra -Werror -Wold-style-cast

client: $(objects)
	g++ $(flags) -o ftp $(objects)

main.o: main.cpp
	g++ $(flags) -c main.cpp

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
	rm -f ftp $(objects)
