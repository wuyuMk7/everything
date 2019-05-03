# Lab7: VPN task1 and task2

## Task1

Linked Clone x 3

- Host U: DHCP
- VPN Server: DHCP + Internal Network (192.168.60.1)
- Host V: (Client) Internal Network (192.168.60.101)

## Task2

### Step1 Server Machine

sudo ./vpnserver
sudo ifconfig tun0 192.168.53.1/24 up
sudo sysctl net.ipv4.ip_forward=1

### Step2 Client Machine (Host U)

sudo ./vpnclient $(HostU_IP)
sudo ifconfig tun0 192.168.53.5/24 up

### Step3 Routing

Client (Host U): sudo route add -net 192.168.60.0/24 tun0

### Step4 Routing on Host V

Host V: sudo route add -net 10.0.2.0/24 gw 192.168.60.1 $(Adapter_name)

### Step5 Test VPN tunnel

Host U: ping 192.168.60.101 & telnet 192.168.60.101

### Step6 Tunnel-Breaking
Break the VPN tunnel and then reconnect it.
