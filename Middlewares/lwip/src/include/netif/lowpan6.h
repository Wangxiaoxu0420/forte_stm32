

#ifndef LWIP_HDR_LOWPAN6_H
#define LWIP_HDR_LOWPAN6_H

#include "netif/lowpan6_opts.h"

#if LWIP_IPV6

#include "netif/lowpan6_common.h"
#include "lwip/pbuf.h"
#include "lwip/ip.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 1 second period for reassembly */
#define LOWPAN6_TMR_INTERVAL 1000

void lowpan6_tmr(void);

err_t lowpan6_set_context(u8_t idx, const ip6_addr_t * context);
err_t lowpan6_set_short_addr(u8_t addr_high, u8_t addr_low);

#if LWIP_IPV4
err_t lowpan4_output(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr);
#endif /* LWIP_IPV4 */
err_t lowpan6_output(struct netif *netif, struct pbuf *q, const ip6_addr_t *ip6addr);
err_t lowpan6_input(struct pbuf * p, struct netif *netif);
err_t lowpan6_if_init(struct netif *netif);

/* pan_id in network byte order. */
err_t lowpan6_set_pan_id(u16_t pan_id);

u16_t lowpan6_calc_crc(const void *buf, u16_t len);

#if !NO_SYS
err_t tcpip_6lowpan_input(struct pbuf *p, struct netif *inp);
#endif /* !NO_SYS */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV6 */

#endif /* LWIP_HDR_LOWPAN6_H */
