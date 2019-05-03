#include "tun.h"

int configIPAddrOnTun(char *dev, char *ip_str, char *mask_str) 
{
  int sockfd;
  struct ifreq ifr;
  struct sockaddr_in *addr, *mask;

  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("Cannot create a socket"); 
    return -1;
  }

  addr = (struct sockaddr_in*)&(ifr.ifr_addr);
  addr->sin_family = AF_INET;
  //inet_pton(AF_INET, "192.168.53.1", &(addr->sin_addr));
  inet_pton(AF_INET, ip_str, &(addr->sin_addr));
  if(ioctl(sockfd, SIOCSIFADDR, &ifr) < 0) {
    perror("Cannot set ip address on tun device");
    close(sockfd);
    return -1;
  }

  mask = (struct sockaddr_in*)&(ifr.ifr_netmask);
  mask->sin_family = AF_INET;
  //inet_pton(AF_INET, "255.255.255.0", &(mask->sin_addr));
  inet_pton(AF_INET, mask_str, &(mask->sin_addr));
  if(ioctl(sockfd, SIOCSIFNETMASK, &ifr) < 0) {
    perror("Cannot set net mask on tun device");
    close(sockfd);
    return -1;
  }

  if(ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
    perror("Cannot enable the tun device");
    close(sockfd);
    return -1;
  }
  ifr.ifr_flags |= (IFF_UP | IFF_RUNNING); 
  if(ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0) {
    perror("Cannot enable the tun device");
    close(sockfd);
    return -1;
  }

  close(sockfd);
  return 0;
}

int configRoutesOnTun(char *dev) 
{
  int sockfd;
  struct rtentry route; 
  struct sockaddr_in *addr;

  memset(&route, 0, sizeof(route));
  route.rt_dev = dev;

  /*
  addr = (struct sockaddr_in*)&route.rt_gateway;
  addr->sin_family = AF_INET;
  inet_pton(AF_INET, "192.168.53.1", &(addr->sin_addr));

  addr = (struct sockaddr_in*)&route.rt_dst;
  addr->sin_family = AF_INET;
  inet_pton(AF_INET, "192.168.53.6", &(addr->sin_addr));

  addr = (struct sockaddr_in*)&route.rt_genmask;
  addr->sin_family = AF_INET;
  inet_pton(AF_INET, "255.255.255.0", &(addr->sin_addr));
  */

  //route.rt_flags = RTF_UP | RTF_REJECT; 
  route.rt_flags = RTF_UP; 
  route.rt_metric = 0;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("Cannot create a socket"); 
    return -1;
  }

  if (ioctl(sockfd, SIOCADDRT, &route) < 0) {
    perror("Cannot set a new route for tun device");
    return -1;
  }
  
  return 0;
}

int configRoutesOnClient(char *dev, char *target_net, char *target_mask) 
{
  int sockfd;
  struct rtentry route; 
  struct sockaddr_in *addr;

  memset(&route, 0, sizeof(route));
  route.rt_dev = dev;

  addr = (struct sockaddr_in*)&route.rt_dst;
  addr->sin_family = AF_INET;
  inet_pton(AF_INET, target_net, &(addr->sin_addr));

  addr = (struct sockaddr_in*)&route.rt_genmask;
  addr->sin_family = AF_INET;
  inet_pton(AF_INET, target_mask, &(addr->sin_addr));

  /*
  addr = (struct sockaddr_in*)&route.rt_gateway;
  addr->sin_family = AF_INET;
  inet_pton(AF_INET, "192.168.53.1", &(addr->sin_addr));

  */

  //route.rt_flags = RTF_UP | RTF_REJECT; 
  route.rt_flags = RTF_UP; 
  route.rt_metric = 0;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("Cannot create a socket"); 
    return -1;
  }

  if (ioctl(sockfd, SIOCADDRT, &route) < 0) {
    perror("Cannot set a new route for tun device");
    return -1;
  }
  
  return 0;
}

/*
 * type = 0 for server, 1 for client
 */
int createTunDevice(char *ip, char *mask, int type) 
{
  int tunfd;
  struct ifreq ifr;

  tunfd = open("/dev/net/tun", O_RDWR);
  if (tunfd < 0) {
    perror("Cannot open a tun file"); 
    exit(-1);
  }

  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI; 
  if(ioctl(tunfd, TUNSETIFF, &ifr) < 0) {
    perror("Cannot create a tun device");
    close(tunfd);
    exit(-1);
  }

  if(configIPAddrOnTun(ifr.ifr_name, ip, mask) < 0) {
    close(tunfd);
    exit(-1);
  }

  if (type == 1) 
    configRoutesOnClient(ifr.ifr_name, extern_target_net, extern_target_mask);

  return tunfd;
}
