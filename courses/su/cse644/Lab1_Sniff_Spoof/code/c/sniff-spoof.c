#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <pcap.h>

#include "packet_headers.h"

void spoof(const u_char *packet)
{
  int sd;
  struct sockaddr_in dest;
  char buffer[1024];

  memset((char*)buffer, 0, 1024);

  int ip_old_len = 0;
  struct ipheader *ip_old;
  struct ipheader *ip;
  struct icmpheader *icmp_old;
  struct icmpheader *icmp;

  ip_old = (struct ipheader*)(packet + sizeof(struct ethernetheader));
  icmp_old = (struct icmpheader*)(packet + sizeof(struct ethernetheader) + sizeof(struct ipheader));

  ip_old_len = ntohs(ip_old->ip_len);
  //printf("size: %hu, %d.\n", ip_old_len, sizeof(struct ipheader) + sizeof(struct icmpheader));

  icmp = (struct icmpheader *)(buffer + sizeof(struct ipheader));
  icmp->icmp_type = 0;
  icmp->icmp_code = icmp_old->icmp_code;
  icmp->icmp_id = icmp_old->icmp_id;
  icmp->icmp_seq = icmp_old->icmp_seq;
  icmp->icmp_chksum = 0;
  icmp->icmp_chksum = in_cksum((unsigned short *)icmp, sizeof(struct icmpheader));

  ip = (struct ipheader *)buffer;
  ip->ip_ver = 4;
  ip->ip_ihl = 5;
  ip->ip_ttl = 20;
  ip->ip_srcip.s_addr = ip_old->ip_dstip.s_addr;
  ip->ip_dstip.s_addr = ip_old->ip_srcip.s_addr;
  ip->ip_proto = IPPROTO_ICMP;
  ip->ip_len = htons(sizeof(struct ipheader) + 
		     sizeof(struct icmpheader));

  if (ntohs(ip->ip_len) < ip_old_len) {
    for (int i = ntohs(ip->ip_len); i < ip_old_len; ++i)
      buffer[i] = *((char*)ip_old+i);
      //buffer[i] = (char)*(packet + sizeof(struct ethernetheader) + i);
    ip->ip_len = htons(ip_old_len);
  }

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
  printf("  1 ICMP echo-reply packet sent.\n");
  close(sd);
}

void got_packet(u_char *args, const struct pcap_pkthdr *header,
	const u_char *packet)
{
  //printf("Got a packet!\n");

  struct ethernetheader *ethernet;

  ethernet = (struct ethernetheader*) packet;
  if (ntohs(ethernet->ethernet_type) == 0x0800) {
    struct ipheader *ip = (struct ipheader*) (packet + sizeof(struct ethernetheader));
    struct icmpheader *icmp;
    
    printf("- Captured IP Packet from %s, ", inet_ntoa(ip->ip_srcip));
    printf("to %s, ", inet_ntoa(ip->ip_dstip));
    switch(ip->ip_proto) {
      case IPPROTO_TCP:
	printf("protocol tcp.\n");
        break;
      case IPPROTO_UDP:
        printf("protocol udp.\n");
        break;
      case IPPROTO_ICMP:
        printf("protocol icmp.\n");
        icmp = (struct icmpheader*) (packet + sizeof(struct ethernetheader) + sizeof(struct ipheader));
    	if ((int)(icmp->icmp_type) == 8)
          spoof(packet);
        break;
      default:
        printf("protocol others.\n");
        break;
    }
  }

  return;
}

int main(int argc, char* argv[])
{
  pcap_t *handle;
  char errbuf[PCAP_ERRBUF_SIZE];
  struct bpf_program fp;
  char filter_exp[] = "icmp";
  bpf_u_int32 net;

  handle = pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf);
  
  pcap_compile(handle, &fp, filter_exp, 0, net);
  pcap_setfilter(handle, &fp);
  
  pcap_loop(handle, -1, got_packet, NULL);
  
  pcap_close(handle);
 
  return 0;
}
