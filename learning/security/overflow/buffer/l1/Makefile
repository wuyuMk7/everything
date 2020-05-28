CFLAGS=-g -Wall -fno-stack-protector -z execstack

all: overflow

overflow: overflow.c
	gcc $(CFLAGS) overflow.c -o overflow

clean:
	rm -rf overflow overflow2
