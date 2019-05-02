#!/usr/bin/env python

from scapy.all import *

def print_pkt(pkt):
  print(pkt.summary())

def sniff_filter(filter):
  sniff(filter=filter, prn=print_pkt)

if __name__ == '__main__':
  type = sys.argv[1] if len(sys.argv) > 1 else 'icmp'

  if type == 'tcp':
    if len(sys.argv) > 2 and sys.argv[2]:
      ip = sys.argv[2]
    else:
      ip = '10.0.2.4'
    filter_str = "tcp and src host %s and dst port 23" % ip
    sniff_filter(filter=filter_str)
  elif type == 'subnet':
    if len(sys.argv) > 2 and sys.argv[2]:
      subnet = sys.argv[2]
    else:
      subnet = '192.168.0.0/24'
    subnet = "net %s" % subnet
    sniff_filter(subnet)
  else:
    sniff_filter(filter='icmp')
