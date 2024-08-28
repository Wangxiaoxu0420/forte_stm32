

#ifndef LWIP_HDR_IGMP_H
#define LWIP_HDR_IGMP_H

#include "lwip/opt.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"

#if LWIP_IPV4 && LWIP_IGMP /* don't build if not configured for use in lwipopts.h */

#ifdef __cplusplus
extern "C" {
#endif

/* IGMP timer */
#define IGMP_TMR_INTERVAL              100 /* Milliseconds */
#define IGMP_V1_DELAYING_MEMBER_TMR   (1000/IGMP_TMR_INTERVAL)
#define IGMP_JOIN_DELAYING_MEMBER_TMR (500 /IGMP_TMR_INTERVAL)

/* Compatibility defines (don't use for new code) */
#define IGMP_DEL_MAC_FILTER            NETIF_DEL_MAC_FILTER
#define IGMP_ADD_MAC_FILTER            NETIF_ADD_MAC_FILTER

/**
 * igmp group structure - there is
 * a list of groups for each interface
 * these should really be linked from the interface, but
 * if we keep them separate we will not affect the lwip original code
 * too much
 *
 * There will be a group for the all systems group address but this
 * will not run the state machine as it is used to kick off reports
 * from all the other groups
 */
struct igmp_group {
  /** next link */
  struct igmp_group *next;
  /** multicast address */
  ip4_addr_t         group_address;
  /** signifies we were the last person to report */
  u8_t               last_reporter_flag;
  /** current state of the group */
  u8_t               group_state;
  /** timer for reporting, negative is OFF */
  u16_t              timer;
  /** counter of simultaneous uses */
  u8_t               use;
};

/*  Prototypes */
void   igmp_init(void);
err_t  igmp_start(struct netif *netif);
err_t  igmp_stop(struct netif *netif);
void   igmp_report_groups(struct netif *netif);
struct igmp_group *igmp_lookfor_group(struct netif *ifp, const ip4_addr_t *addr);
void   igmp_input(struct pbuf *p, struct netif *inp, const ip4_addr_t *dest);
err_t  igmp_joingroup(const ip4_addr_t *ifaddr, const ip4_addr_t *groupaddr);
err_t  igmp_joingroup_netif(struct netif *netif, const ip4_addr_t *groupaddr);
err_t  igmp_leavegroup(const ip4_addr_t *ifaddr, const ip4_addr_t *groupaddr);
err_t  igmp_leavegroup_netif(struct netif *netif, const ip4_addr_t *groupaddr);
void   igmp_tmr(void);

/** @ingroup igmp 
 * Get list head of IGMP groups for netif.
 * Note: The allsystems group IP is contained in the list as first entry.
 * @see @ref netif_set_igmp_mac_filter()
 */
#define netif_igmp_data(netif) ((struct igmp_group *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_IGMP))

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV4 && LWIP_IGMP */

#endif /* LWIP_HDR_IGMP_H */
