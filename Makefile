CC=g++
CFLAGS=-I. -std=c++11



client: client.cpp requests.cpp helpers.cpp 
	$(CC) -o client client.cpp requests.cpp helpers.cpp -Wall

run: client
	./client

clean:
	rm -f *.o client
