
 
#ifndef LWIP_HDR_LOWPAN6_BLE_H
#define LWIP_HDR_LOWPAN6_BLE_H

#include "netif/lowpan6_opts.h"

#if LWIP_IPV6 /* don't build if not configured for use in lwipopts.h */

#include "netif/lowpan6_common.h"
#include "lwip/pbuf.h"
#include "lwip/ip.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

err_t rfc7668_output(struct netif *netif, struct pbuf *q, const ip6_addr_t *ip6addr);
err_t rfc7668_input(struct pbuf * p, struct netif *netif);
err_t rfc7668_set_local_addr_eui64(struct netif *netif, const u8_t *local_addr, size_t local_addr_len);
err_t rfc7668_set_local_addr_mac48(struct netif *netif, const u8_t *local_addr, size_t local_addr_len, int is_public_addr);
err_t rfc7668_set_peer_addr_eui64(struct netif *netif, const u8_t *peer_addr, size_t peer_addr_len);
err_t rfc7668_set_peer_addr_mac48(struct netif *netif, const u8_t *peer_addr, size_t peer_addr_len, int is_public_addr);
err_t rfc7668_set_context(u8_t index, const ip6_addr_t * context);
err_t rfc7668_if_init(struct netif *netif);

#if !NO_SYS
err_t tcpip_rfc7668_input(struct pbuf *p, struct netif *inp);
#endif

void ble_addr_to_eui64(uint8_t *dst, const uint8_t *src, int public_addr);
void eui64_to_ble_addr(uint8_t *dst, const uint8_t *src);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV6 */

#endif /* LWIP_HDR_LOWPAN6_BLE_H */
