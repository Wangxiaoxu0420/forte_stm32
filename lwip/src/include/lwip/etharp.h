

#ifndef LWIP_HDR_NETIF_ETHARP_H
#define LWIP_HDR_NETIF_ETHARP_H

#include "lwip/opt.h"

#if LWIP_ARP || LWIP_ETHERNET /* don't build if not configured for use in lwipopts.h */

#include "lwip/pbuf.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/ip4.h"
#include "lwip/prot/ethernet.h"

#if LWIP_IPV4 && LWIP_ARP /* don't build if not configured for use in lwipopts.h */

#include "lwip/prot/etharp.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 1 seconds period */
#define ARP_TMR_INTERVAL 1000

#if ARP_QUEUEING
/** struct for queueing outgoing packets for unknown address
  * defined here to be accessed by memp.h
  */
struct etharp_q_entry {
  struct etharp_q_entry *next;
  struct pbuf *p;
};
#endif /* ARP_QUEUEING */

#define etharp_init() /* Compatibility define, no init needed. */
void etharp_tmr(void);
ssize_t etharp_find_addr(struct netif *netif, const ip4_addr_t *ipaddr,
         struct eth_addr **eth_ret, const ip4_addr_t **ip_ret);
int etharp_get_entry(size_t i, ip4_addr_t **ipaddr, struct netif **netif, struct eth_addr **eth_ret);
err_t etharp_output(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr);
err_t etharp_query(struct netif *netif, const ip4_addr_t *ipaddr, struct pbuf *q);
err_t etharp_request(struct netif *netif, const ip4_addr_t *ipaddr);
/** For Ethernet network interfaces, we might want to send "gratuitous ARP";
 *  this is an ARP packet sent by a node in order to spontaneously cause other
 *  nodes to update an entry in their ARP cache.
 *  From RFC 3220 "IP Mobility Support for IPv4" section 4.6. */
#define etharp_gratuitous(netif) etharp_request((netif), netif_ip4_addr(netif))
void etharp_cleanup_netif(struct netif *netif);

#if ETHARP_SUPPORT_STATIC_ENTRIES
err_t etharp_add_static_entry(const ip4_addr_t *ipaddr, struct eth_addr *ethaddr);
err_t etharp_remove_static_entry(const ip4_addr_t *ipaddr);
#endif /* ETHARP_SUPPORT_STATIC_ENTRIES */

void etharp_input(struct pbuf *p, struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV4 && LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */

#endif /* LWIP_HDR_NETIF_ETHARP_H */
