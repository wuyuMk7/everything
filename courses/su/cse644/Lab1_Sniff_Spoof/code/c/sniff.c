#include <stdio.h>
#include <arpa/inet.h>
#include <pcap.h>

#include "packet_headers.h"

void got_packet(u_char *args, const struct pcap_pkthdr *header,
	const u_char *packet)
{
  printf("Got a packet!\n");

  struct ethernetheader *ethernet;

  ethernet = (struct ethernetheader*) packet;
  if (ntohs(ethernet->ethernet_type) == 0x0800) {
    struct ipheader *ip = (struct ipheader*) (packet + sizeof(struct ethernetheader));
    
    printf("- IP Packet from %s,", inet_ntoa(ip->ip_srcip));
    printf(" to %s, ", inet_ntoa(ip->ip_dstip));
    switch(ip->ip_proto) {
      case IPPROTO_TCP:
	printf("protocol tcp.\n");
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
  char filter_exp[] = "(icmp and host 192.168.0.103 and host 10.0.2.4) or (tcp portrange 10-100)";
  bpf_u_int32 net;

  handle = pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf);
  
  pcap_compile(handle, &fp, filter_exp, 0, net);
  pcap_setfilter(handle, &fp);
  
  pcap_loop(handle, -1, got_packet, NULL);
  
  pcap_close(handle);
 
  return 0;
}
