
#ifndef LWIP_HDR_ICMP6_H
#define LWIP_HDR_ICMP6_H

#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
#include "lwip/prot/icmp6.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_ICMP6 && LWIP_IPV6 /* don't build if not configured for use in lwipopts.h */

void icmp6_input(struct pbuf *p, struct netif *inp);
void icmp6_dest_unreach(struct pbuf *p, enum icmp6_dur_code c);
void icmp6_packet_too_big(struct pbuf *p, u32_t mtu);
void icmp6_time_exceeded(struct pbuf *p, enum icmp6_te_code c);
void icmp6_time_exceeded_with_addrs(struct pbuf *p, enum icmp6_te_code c,
    const ip6_addr_t *src_addr, const ip6_addr_t *dest_addr);
void icmp6_param_problem(struct pbuf *p, enum icmp6_pp_code c, const void *pointer);

#endif /* LWIP_ICMP6 && LWIP_IPV6 */


#ifdef __cplusplus
}
#endif


#endif /* LWIP_HDR_ICMP6_H */
