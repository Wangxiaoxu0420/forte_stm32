
#ifndef LWIP_HDR_IP6_H
#define LWIP_HDR_IP6_H

#include "lwip/opt.h"

#if LWIP_IPV6  /* don't build if not configured for use in lwipopts.h */

#include "lwip/ip6_addr.h"
#include "lwip/prot/ip6.h"
#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"

#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

struct netif *ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest);
const ip_addr_t *ip6_select_source_address(struct netif *netif, const ip6_addr_t * dest);
err_t         ip6_input(struct pbuf *p, struct netif *inp);
err_t         ip6_output(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
                         u8_t hl, u8_t tc, u8_t nexth);
err_t         ip6_output_if(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
                            u8_t hl, u8_t tc, u8_t nexth, struct netif *netif);
err_t         ip6_output_if_src(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
                            u8_t hl, u8_t tc, u8_t nexth, struct netif *netif);
#if LWIP_NETIF_USE_HINTS
err_t         ip6_output_hinted(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
                                u8_t hl, u8_t tc, u8_t nexth, struct netif_hint *netif_hint);
#endif /* LWIP_NETIF_USE_HINTS */
#if LWIP_IPV6_MLD
err_t         ip6_options_add_hbh_ra(struct pbuf * p, u8_t nexth, u8_t value);
#endif /* LWIP_IPV6_MLD */

#define ip6_netif_get_local_ip(netif, dest) (((netif) != NULL) ? \
  ip6_select_source_address(netif, dest) : NULL)

#if IP6_DEBUG
void ip6_debug_print(struct pbuf *p);
#else
#define ip6_debug_print(p)
#endif /* IP6_DEBUG */


#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV6 */

#endif /* LWIP_HDR_IP6_H */
