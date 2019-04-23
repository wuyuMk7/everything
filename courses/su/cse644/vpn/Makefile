all: server client

server: server.c tun.o
	gcc -o server server.c tun.o -lssl -lcrypto -lcrypt

client: client.c tun.o
	gcc -o client client.c tun.o -lssl -lcrypto -lcrypt

tun.o: tun.c tun.h
	gcc -o tun.o -c tun.c

clean:
	rm -rf *.o server client
