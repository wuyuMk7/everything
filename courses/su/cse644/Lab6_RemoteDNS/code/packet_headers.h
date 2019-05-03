#ifndef _PACKET_HEADER_H_
#define _PACKET_HEADER_H_

#define ETHER_ADDR_LEN 6

struct ethernetheader {
  unsigned char ethernet_dstmac[ETHER_ADDR_LEN];
  unsigned char ethernet_srcmac[ETHER_ADDR_LEN];
  unsigned short ethernet_type;
};

struct icmpheader {
  unsigned char icmp_type;
  unsigned char icmp_code;
  unsigned short icmp_chksum;
  unsigned short icmp_id;
  unsigned short icmp_seq;
};

struct ipheader {
  unsigned char ip_ihl: 4,
	        ip_ver: 4;
  unsigned char ip_tos;
  unsigned short ip_len;
  unsigned short ip_id;
  unsigned short ip_flag: 3,
   		 ip_offset: 13;
  unsigned char ip_ttl;
  unsigned char ip_proto;
  unsigned short ip_chksum;

  struct in_addr ip_srcip;
  struct in_addr ip_dstip;
};

struct tcpheader {
  unsigned short tcp_sport;
  unsigned short tcp_dport;
  unsigned int tcp_seq;
  unsigned int tcp_ack;
  unsigned char tcp_off;
#define TH_OFF(th) (((th)->tcp_off & 0xf0) >> 4)
  unsigned char tcp_flags;
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
#define TH_ECE 0x40
#define TH_CWR 0x80
#define TH_FALGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
  unsigned short tcp_win;
  unsigned short tcp_chksum;
  unsigned short tcp_urp;
};

unsigned short in_cksum(unsigned short *buf, int length)
{
  unsigned short *w = buf;
  int nleft = length;
  int sum = 0;
  unsigned short temp = 0;

  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }

  if (nleft == 1) {
    *(u_char *)(&temp) = *(u_char *)w;
    sum += temp;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return (unsigned short)(~sum);
}

#endif
