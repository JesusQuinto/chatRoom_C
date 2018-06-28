all : chat_server client

CPPFLAGS = -g -I.

chat_server : chat_server.c
	cc -g -I. chat_server.c -O2 -lpthread -o chat_server

client : client.c
	cc -g -I. client.c -lpthread -o client

clean :
	rm client chat_server