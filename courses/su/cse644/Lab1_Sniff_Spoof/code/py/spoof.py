#!/usr/bin/env python
from scapy.all import *

def spoof_icmp(ip='127.0.0.1'):
  ip_packet = IP()
  ip_packet.dst = ip
  icmp_packet = ICMP()
  send(ip_packet/icmp_packet)

if __name__ == '__main__':
  dst = sys.argv[1] if len(sys.argv) > 1 else '127.0.0.1'

  for i in range(5):
    spoof_icmp(ip=dst)
  
