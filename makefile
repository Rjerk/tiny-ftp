objects = main.o ftpClient.o userPI.o error.o utility.o

flags = -O -Wall -std=c++11

client: $(objects)
	g++ $(flags) -o client $(objects)

main.o: main.cpp
	g++ $(flags) -c main.cpp

ftpClient.o: ftpClient.cpp
	g++ $(flags) -c ftpClient.cpp

userPI.o: userPI.cpp 
	g++ $(flags) -c userPI.cpp

utility.o: utility.cpp
	g++ $(flags) -c utility.cpp

error.o: error.cpp
	g++ $(flags) -c error.cpp

.PHONY: clean

clean:
	rm -f client $(objects)
