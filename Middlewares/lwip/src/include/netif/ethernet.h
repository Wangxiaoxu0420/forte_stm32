

#ifndef LWIP_HDR_NETIF_ETHERNET_H
#define LWIP_HDR_NETIF_ETHERNET_H

#include "lwip/opt.h"

#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/prot/ethernet.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_ARP || LWIP_ETHERNET

/** Define this to 1 and define LWIP_ARP_FILTER_NETIF_FN(pbuf, netif, type)
 * to a filter function that returns the correct netif when using multiple
 * netifs on one hardware interface where the netif's low-level receive
 * routine cannot decide for the correct netif (e.g. when mapping multiple
 * IP addresses to one hardware interface).
 */
#ifndef LWIP_ARP_FILTER_NETIF
#define LWIP_ARP_FILTER_NETIF 0
#endif

err_t ethernet_input(struct pbuf *p, struct netif *netif);
err_t ethernet_output(struct netif* netif, struct pbuf* p, const struct eth_addr* src, const struct eth_addr* dst, u16_t eth_type);

extern const struct eth_addr ethbroadcast, ethzero;

#endif /* LWIP_ARP || LWIP_ETHERNET */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_NETIF_ETHERNET_H */
