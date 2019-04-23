#ifndef __TUN_H__
#define __TUN_H__

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/route.h>
#include <sys/ioctl.h>

// Should only be used for client
extern char* extern_target_net;
extern char* extern_target_mask;

int configIPAddrOnTun(char*, char*, char*);
int configRoutesOnTun(char*);
int configRoutesOnClient(char*, char*, char*);
int createTunDevice(char*, char*, int);

#endif
