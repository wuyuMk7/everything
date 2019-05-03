#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "packet_headers.h"

void spoof()
{
  int sd;
  struct sockaddr_in dest;
  char buffer[1024];

  memset((char*)buffer, 0, 1024);

  struct icmpheader *icmp = (struct icmpheader *)(buffer + sizeof(struct ipheader));
  icmp->icmp_type = 8;
  icmp->icmp_chksum = 0;
  icmp->icmp_chksum = in_cksum((unsigned short *)icmp, sizeof(struct icmpheader));

  struct ipheader *ip = (struct ipheader *)buffer;
  ip->ip_ver = 4;
  ip->ip_ihl = 5;
  ip->ip_ttl = 20;
  ip->ip_srcip.s_addr = inet_addr("10.0.2.15");
  ip->ip_dstip.s_addr = inet_addr("10.0.2.4");
  ip->ip_proto = IPPROTO_ICMP;
  ip->ip_len = htons(sizeof(struct ipheader) + 
		     sizeof(struct icmpheader));

  sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
  if (sd < 0) {
    perror("socket() error");
    exit(-1);
  }

  dest.sin_family = AF_INET;
  dest.sin_addr = ip->ip_dstip;
 
  if (sendto(sd, buffer, ntohs(ip->ip_len), 0, 
      	(struct sockaddr *) &dest, sizeof(dest)) < 0) {
    perror("sendto() error"); 
    exit(-1);
  }
  close(sd);
}

int main(int argc, char* argv[])
{
  spoof();

  return 0;
}
