#!/usr/bin/env python
from scapy.all import *

if __name__ == '__main__':
  dst = sys.argv[1] if len(sys.argv) > 1 else 'www.syracuse.edu'

  ip_packet = IP()
  ip_packet.ttl = 0
  ip_packet.dst = dst
  
  icmp_packet = ICMP()
  
  for i in range(20):
    ip_packet.ttl += 1
    resp = sr1(ip_packet/icmp_packet, verbose=0, timeout=2)
    if resp:
      print('%d %s' % (i + 1, resp[IP].src))
    else:
      print('%d *' % (i + 1))
      continue

    icmp_type = resp[ICMP].type
    if icmp_type == 0:
        print("Traceroute finished.")
	break
    elif icmp_type == 11:
	pass
    else:
	print("Traceroute failed.")
	break
