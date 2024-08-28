

#ifndef LWIP_HDR_MLD6_H
#define LWIP_HDR_MLD6_H

#include "lwip/opt.h"

#if LWIP_IPV6_MLD && LWIP_IPV6  /* don't build if not configured for use in lwipopts.h */

#include "lwip/pbuf.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/** MLD group */
struct mld_group {
  /** next link */
  struct mld_group *next;
  /** multicast address */
  ip6_addr_t         group_address;
  /** signifies we were the last person to report */
  u8_t               last_reporter_flag;
  /** current state of the group */
  u8_t               group_state;
  /** timer for reporting */
  u16_t              timer;
  /** counter of simultaneous uses */
  u8_t               use;
};

#define MLD6_TMR_INTERVAL              100 /* Milliseconds */

err_t  mld6_stop(struct netif *netif);
void   mld6_report_groups(struct netif *netif);
void   mld6_tmr(void);
struct mld_group *mld6_lookfor_group(struct netif *ifp, const ip6_addr_t *addr);
void   mld6_input(struct pbuf *p, struct netif *inp);
err_t  mld6_joingroup(const ip6_addr_t *srcaddr, const ip6_addr_t *groupaddr);
err_t  mld6_joingroup_netif(struct netif *netif, const ip6_addr_t *groupaddr);
err_t  mld6_leavegroup(const ip6_addr_t *srcaddr, const ip6_addr_t *groupaddr);
err_t  mld6_leavegroup_netif(struct netif *netif, const ip6_addr_t *groupaddr);

/** @ingroup mld6
 * Get list head of MLD6 groups for netif.
 * Note: The allnodes group IP is NOT in the list, since it must always 
 * be received for correct IPv6 operation.
 * @see @ref netif_set_mld_mac_filter()
 */
#define netif_mld6_data(netif) ((struct mld_group *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_MLD6))

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV6_MLD && LWIP_IPV6 */

#endif /* LWIP_HDR_MLD6_H */
