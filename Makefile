all : server client

CPPFLAGS = -g -I.

server : server.c
	cc -g -I. server.c -w -O2 -lpthread -o server.o

client : client.c
	cc -g -I. client.c -lpthread -o client.o

clean :
	rm client.o server.o