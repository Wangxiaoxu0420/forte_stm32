

#ifndef LWIP_HDR_ZEPIF_H
#define LWIP_HDR_ZEPIF_H

#include "lwip/opt.h"
#include "netif/lowpan6.h"

#if LWIP_IPV6 && LWIP_UDP /* don't build if not configured for use in lwipopts.h */

#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ZEPIF_DEFAULT_UDP_PORT  17754

/** Pass this struct as 'state' to netif_add to control the behaviour
 * of this netif. If NULL is passed, default behaviour is chosen */
struct zepif_init {
  /** The UDP port used to ZEP frames from (0 = default) */
  u16_t               zep_src_udp_port;
  /** The UDP port used to ZEP frames to (0 = default) */
  u16_t               zep_dst_udp_port;
  /** The IP address to sed ZEP frames from (NULL = ANY) */
  const ip_addr_t    *zep_src_ip_addr;
  /** The IP address to sed ZEP frames to (NULL = BROADCAST) */
  const ip_addr_t    *zep_dst_ip_addr;
  /** If != NULL, the udp pcb is bound to this netif */
  const struct netif *zep_netif;
  /** MAC address of the 6LowPAN device */
  u8_t                addr[6];
};

err_t zepif_init(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV6 && LWIP_UDP */

#endif /* LWIP_HDR_ZEPIF_H */
