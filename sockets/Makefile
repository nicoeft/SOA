all: serverSocket clientSocket


serverSocket: serverSocket.c socketMng.o
	gcc -o serverSocket -g serverSocket.c socketMng.o -pthread

clientSocket: clientSocket.c socketMng.o
	gcc -o clientSocket -g clientSocket.c socketMng.o

socketMng.o: socketMng.c
	gcc -c -g socketMng.c

clean: 
	rm -f serverSocket clientSocket socketMng.o