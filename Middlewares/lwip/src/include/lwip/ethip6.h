

#ifndef LWIP_HDR_ETHIP6_H
#define LWIP_HDR_ETHIP6_H

#include "lwip/opt.h"

#if LWIP_IPV6 && LWIP_ETHERNET /* don't build if not configured for use in lwipopts.h */

#include "lwip/pbuf.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"


#ifdef __cplusplus
extern "C" {
#endif


err_t ethip6_output(struct netif *netif, struct pbuf *q, const ip6_addr_t *ip6addr);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV6 && LWIP_ETHERNET */

#endif /* LWIP_HDR_ETHIP6_H */
