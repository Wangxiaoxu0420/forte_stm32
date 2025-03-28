

#ifndef LWIP_HDR_AUTOIP_H
#define LWIP_HDR_AUTOIP_H

#include "lwip/opt.h"

#if LWIP_IPV4 && LWIP_AUTOIP /* don't build if not configured for use in lwipopts.h */

#include "lwip/netif.h"
/* #include "lwip/udp.h" */
#include "lwip/etharp.h"

#ifdef __cplusplus
extern "C" {
#endif

/** AutoIP Timing */
#define AUTOIP_TMR_INTERVAL      100
#define AUTOIP_TICKS_PER_SECOND (1000 / AUTOIP_TMR_INTERVAL)

/** AutoIP state information per netif */
struct autoip
{
  /** the currently selected, probed, announced or used LL IP-Address */
  ip4_addr_t llipaddr;
  /** current AutoIP state machine state */
  u8_t state;
  /** sent number of probes or announces, dependent on state */
  u8_t sent_num;
  /** ticks to wait, tick is AUTOIP_TMR_INTERVAL long */
  u16_t ttw;
  /** ticks until a conflict can be solved by defending */
  u8_t lastconflict;
  /** total number of probed/used Link Local IP-Addresses */
  u8_t tried_llipaddr;
};


void autoip_set_struct(struct netif *netif, struct autoip *autoip);
/** Remove a struct autoip previously set to the netif using autoip_set_struct() */
#define autoip_remove_struct(netif) do { (netif)->autoip = NULL; } while (0)
err_t autoip_start(struct netif *netif);
err_t autoip_stop(struct netif *netif);
void autoip_arp_reply(struct netif *netif, struct etharp_hdr *hdr);
void autoip_tmr(void);
void autoip_network_changed(struct netif *netif);
u8_t autoip_supplied_address(const struct netif *netif);

/* for lwIP internal use by ip4.c */
u8_t autoip_accept_packet(struct netif *netif, const ip4_addr_t *addr);

#define netif_autoip_data(netif) ((struct autoip*)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_AUTOIP))

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV4 && LWIP_AUTOIP */

#endif /* LWIP_HDR_AUTOIP_H */
