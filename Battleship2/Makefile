all: client server

client: client.c
	gcc client.c -o client

server: server.c
	gcc -pthread server.c -o server

clean:
	rm -rf client server
