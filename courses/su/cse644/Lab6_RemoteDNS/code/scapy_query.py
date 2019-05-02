#!/usr/bin/env python

from scapy.all import *
conf.L3socket=L3RawSocket

ip = IP(src="10.0.2.15", dst="10.0.2.5")
udp = UDP(sport=33333, dport=53)

target_name = "aaaaa.example.com"
dns = DNS(qd=DNSQR(qname=target_name)) 

spoof_pkt = ip/udp/dns
#send(spoof_pkt)

with open("query.bin", "wb") as f:
  f.write(bytes(spoof_pkt))
