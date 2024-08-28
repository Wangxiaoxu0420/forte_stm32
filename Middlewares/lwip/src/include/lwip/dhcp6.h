

#ifndef LWIP_HDR_IP6_DHCP6_H
#define LWIP_HDR_IP6_DHCP6_H

#include "lwip/opt.h"

#if LWIP_IPV6_DHCP6  /* don't build if not configured for use in lwipopts.h */

#include "lwip/err.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/** period (in milliseconds) of the application calling dhcp6_tmr() */
#define DHCP6_TIMER_MSECS   500

struct dhcp6
{
  /** transaction identifier of last sent request */
  u32_t xid;
  /** track PCB allocation state */
  u8_t pcb_allocated;
  /** current DHCPv6 state machine state */
  u8_t state;
  /** retries of current request */
  u8_t tries;
  /** if request config is triggered while another action is active, this keeps track of it */
  u8_t request_config_pending;
  /** #ticks with period DHCP6_TIMER_MSECS for request timeout */
  u16_t request_timeout;
#if LWIP_IPV6_DHCP6_STATEFUL
  /* @todo: add more members here to keep track of stateful DHCPv6 data, like lease times */
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
};

void dhcp6_set_struct(struct netif *netif, struct dhcp6 *dhcp6);
/** Remove a struct dhcp6 previously set to the netif using dhcp6_set_struct() */
#define dhcp6_remove_struct(netif) netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6, NULL)
void dhcp6_cleanup(struct netif *netif);

err_t dhcp6_enable_stateful(struct netif *netif);
err_t dhcp6_enable_stateless(struct netif *netif);
void dhcp6_disable(struct netif *netif);

void dhcp6_tmr(void);

void dhcp6_nd6_ra_trigger(struct netif *netif, u8_t managed_addr_config, u8_t other_config);

#if LWIP_DHCP6_GET_NTP_SRV
/** This function must exist, in other to add offered NTP servers to
 * the NTP (or SNTP) engine.
 * See LWIP_DHCP6_MAX_NTP_SERVERS */
extern void dhcp6_set_ntp_servers(u8_t num_ntp_servers, const ip_addr_t* ntp_server_addrs);
#endif /* LWIP_DHCP6_GET_NTP_SRV */

#define netif_dhcp6_data(netif) ((struct dhcp6*)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6))

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV6_DHCP6 */

#endif /* LWIP_HDR_IP6_DHCP6_H */
