

#ifndef LWIP_HDR_LOWPAN6_COMMON_H
#define LWIP_HDR_LOWPAN6_COMMON_H

#include "netif/lowpan6_opts.h"

#if LWIP_IPV6 /* don't build if IPv6 is disabled in lwipopts.h */

#include "lwip/pbuf.h"
#include "lwip/ip.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Helper define for a link layer address, which can be encoded as 0, 2 or 8 bytes */
struct lowpan6_link_addr {
  /* encoded length of the address */
  u8_t addr_len;
  /* address bytes */
  u8_t addr[8];
};

s8_t lowpan6_get_address_mode(const ip6_addr_t *ip6addr, const struct lowpan6_link_addr *mac_addr);

#if LWIP_6LOWPAN_IPHC
err_t lowpan6_compress_headers(struct netif *netif, u8_t *inbuf, size_t inbuf_size, u8_t *outbuf, size_t outbuf_size,
                               u8_t *lowpan6_header_len_out, u8_t *hidden_header_len_out, ip6_addr_t *lowpan6_contexts,
                               const struct lowpan6_link_addr *src, const struct lowpan6_link_addr *dst);
struct pbuf *lowpan6_decompress(struct pbuf *p, u16_t datagram_size, ip6_addr_t *lowpan6_contexts,
                                struct lowpan6_link_addr *src, struct lowpan6_link_addr *dest);
#endif /* LWIP_6LOWPAN_IPHC */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV6 */

#endif /* LWIP_HDR_LOWPAN6_COMMON_H */
