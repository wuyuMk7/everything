CFLAGS=-g -Wall
LIBS=-lpcap

executables=sniff spoof sniff-spoof telnet-sniff

all: $(executables)

$(executables): % : %.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

clean:
	rm $(executables)
