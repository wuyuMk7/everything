CFLAGS=-g -Wall
LIBS=-lpcap

executables=remote_dns

all: $(executables)

$(executables): % : %.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

clean:
	rm $(executables)
