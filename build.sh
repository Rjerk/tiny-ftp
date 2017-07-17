#!/bin/bash

g++ -std=c++11 -O -Wall -Wextra -Wold-style-cast error.cpp utility.cpp Socket.cpp InetAddr.cpp Acceptor.cpp TcpStream.cpp UserPI.cpp UserDTP.cpp FtpHandler.cpp FtpClient.cpp main.cpp -o client
