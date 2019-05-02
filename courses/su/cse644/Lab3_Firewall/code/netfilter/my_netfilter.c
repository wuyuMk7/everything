#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/inet.h>

static struct nf_hook_ops in_filters_hook;
static struct nf_hook_ops post_routing_filters_hook;

void prn_pkt(char *info, unsigned char* saddr, unsigned char *daddr)
{
  printk(KERN_INFO "%s from %d.%d.%d.%d to %d.%d.%d.%d\n",
    info,
    saddr[0], saddr[1], saddr[2], saddr[3],
    daddr[0], daddr[1], daddr[2], daddr[3]
  );
}

unsigned int in_filter_hook_function(
	void *priv,
	struct sk_buff *skb,
	const struct nf_hook_state *state)
{
  struct iphdr *iph;
  struct tcphdr *tcph;
  int host_B_ip = in_aton("10.0.2.4");

  iph = ip_hdr(skb);
  tcph = (void*)iph + iph->ihl * 4;

  if (iph->protocol == IPPROTO_TCP && 
      ntohs(tcph->dest) == 23 && 
      iph->saddr == host_B_ip) {
    prn_pkt("(IN) Dropping telnet packet:", (unsigned char*)&iph->saddr, (unsigned char*)&iph->daddr);
    return NF_DROP;
  } else if (iph->protocol == IPPROTO_ICMP && iph->saddr == host_B_ip) {
    prn_pkt("(IN) Dropping ICMP packets:", (unsigned char*)&iph->saddr, (unsigned char*)&iph->daddr);
    return NF_DROP;
  } else if (iph->protocol == IPPROTO_ICMP && iph->tot_len > htons(128)) {
    prn_pkt("(IN) Dropping ICMP packets larger than 128 Bytes (including headers):", (unsigned char*)&iph->saddr, (unsigned char*)&iph->daddr);
    return NF_DROP;
  } else {
    return NF_ACCEPT;
  }
}

unsigned int post_routing_filter_hook_function(
	void *priv,
	struct sk_buff *skb,
	const struct nf_hook_state *state)
{
  struct iphdr *iph;
  struct tcphdr *tcph;
  int host_B_ip = (in_aton("10.0.2.4"));
  int facebook_ip = in_aton("157.240.18.35");

  iph = ip_hdr(skb);
  tcph = (void*)iph + iph->ihl * 4;

  if (iph->protocol == IPPROTO_TCP && 
      ntohs(tcph->dest) == 23 && 
      iph->daddr == host_B_ip) {
    prn_pkt("(POST-ROUTING) Dropping telnet packets:", (unsigned char*)&iph->saddr, (unsigned char*)&iph->daddr);
    return NF_DROP;
  } else if (iph->protocol == IPPROTO_TCP && 
      (ntohs(tcph->dest) == 80 || ntohs(tcph->dest) == 443)&& 
      iph->daddr == facebook_ip) {
    prn_pkt("(POST-ROUTING) Dropping http packets to www.facebook.com:", (unsigned char*)&iph->saddr, (unsigned char*)&iph->daddr);
    return NF_DROP;
  } else {
    return NF_ACCEPT;
  }
}

int setUpFilter(void) 
{
  printk(KERN_INFO "Registering my net filters.\n");

  in_filters_hook.hook = in_filter_hook_function;
  in_filters_hook.hooknum = NF_INET_LOCAL_IN;
  in_filters_hook.pf = PF_INET;
  in_filters_hook.priority = NF_IP_PRI_FIRST;

  post_routing_filters_hook.hook = post_routing_filter_hook_function;
  post_routing_filters_hook.hooknum = NF_INET_POST_ROUTING;
  post_routing_filters_hook.pf = PF_INET;
  post_routing_filters_hook.priority = NF_IP_PRI_FIRST;

  nf_register_hook(&in_filters_hook);
  nf_register_hook(&post_routing_filters_hook);

  return 0;
}

void removeFilter(void)
{
  printk(KERN_INFO "My net filters are being removed.\n");
  nf_unregister_hook(&in_filters_hook);
  nf_unregister_hook(&post_routing_filters_hook);
}

module_init(setUpFilter);
module_exit(removeFilter);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yang He");
MODULE_DESCRIPTION("Filters for the firewall."); 
