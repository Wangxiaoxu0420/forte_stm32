

#ifndef LWIP_HDR_ND6_H
#define LWIP_HDR_ND6_H

#include "lwip/opt.h"

#if LWIP_IPV6  /* don't build if not configured for use in lwipopts.h */

#include "lwip/ip6_addr.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 1 second period */
#define ND6_TMR_INTERVAL 1000

/** Router solicitations are sent in 4 second intervals (see RFC 4861, ch. 6.3.7) */
#ifndef ND6_RTR_SOLICITATION_INTERVAL
#define ND6_RTR_SOLICITATION_INTERVAL  4000
#endif

struct pbuf;
struct netif;

void nd6_tmr(void);
void nd6_input(struct pbuf *p, struct netif *inp);
void nd6_clear_destination_cache(void);
struct netif *nd6_find_route(const ip6_addr_t *ip6addr);
err_t nd6_get_next_hop_addr_or_queue(struct netif *netif, struct pbuf *q, const ip6_addr_t *ip6addr, const u8_t **hwaddrp);
u16_t nd6_get_destination_mtu(const ip6_addr_t *ip6addr, struct netif *netif);
#if LWIP_ND6_TCP_REACHABILITY_HINTS
void nd6_reachability_hint(const ip6_addr_t *ip6addr);
#endif /* LWIP_ND6_TCP_REACHABILITY_HINTS */
void nd6_cleanup_netif(struct netif *netif);
#if LWIP_IPV6_MLD
void nd6_adjust_mld_membership(struct netif *netif, s8_t addr_idx, u8_t new_state);
#endif /* LWIP_IPV6_MLD */
void nd6_restart_netif(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV6 */

#endif /* LWIP_HDR_ND6_H */
