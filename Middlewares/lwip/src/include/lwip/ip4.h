
#ifndef LWIP_HDR_IP4_H
#define LWIP_HDR_IP4_H

#include "lwip/opt.h"

#if LWIP_IPV4

#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/ip4_addr.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "lwip/prot/ip4.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LWIP_HOOK_IP4_ROUTE_SRC
#define LWIP_IPV4_SRC_ROUTING   1
#else
#define LWIP_IPV4_SRC_ROUTING   0
#endif

/** Currently, the function ip_output_if_opt() is only used with IGMP */
#define IP_OPTIONS_SEND   (LWIP_IPV4 && LWIP_IGMP)

#define ip_init() /* Compatibility define, no init needed. */
struct netif *ip4_route(const ip4_addr_t *dest);
#if LWIP_IPV4_SRC_ROUTING
struct netif *ip4_route_src(const ip4_addr_t *src, const ip4_addr_t *dest);
#else /* LWIP_IPV4_SRC_ROUTING */
#define ip4_route_src(src, dest) ip4_route(dest)
#endif /* LWIP_IPV4_SRC_ROUTING */
err_t ip4_input(struct pbuf *p, struct netif *inp);
err_t ip4_output(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
       u8_t ttl, u8_t tos, u8_t proto);
err_t ip4_output_if(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
       u8_t ttl, u8_t tos, u8_t proto, struct netif *netif);
err_t ip4_output_if_src(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
       u8_t ttl, u8_t tos, u8_t proto, struct netif *netif);
#if LWIP_NETIF_USE_HINTS
err_t ip4_output_hinted(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
       u8_t ttl, u8_t tos, u8_t proto, struct netif_hint *netif_hint);
#endif /* LWIP_NETIF_USE_HINTS */
#if IP_OPTIONS_SEND
err_t ip4_output_if_opt(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
       u8_t ttl, u8_t tos, u8_t proto, struct netif *netif, void *ip_options,
       u16_t optlen);
err_t ip4_output_if_opt_src(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
       u8_t ttl, u8_t tos, u8_t proto, struct netif *netif, void *ip_options,
       u16_t optlen);
#endif /* IP_OPTIONS_SEND */

#if LWIP_MULTICAST_TX_OPTIONS
void  ip4_set_default_multicast_netif(struct netif* default_multicast_netif);
#endif /* LWIP_MULTICAST_TX_OPTIONS */

#define ip4_netif_get_local_ip(netif) (((netif) != NULL) ? netif_ip_addr4(netif) : NULL)

#if IP_DEBUG
void ip4_debug_print(struct pbuf *p);
#else
#define ip4_debug_print(p)
#endif /* IP_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV4 */

#endif /* LWIP_HDR_IP_H */


