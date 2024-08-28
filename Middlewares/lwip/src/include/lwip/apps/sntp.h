
#ifndef LWIP_HDR_APPS_SNTP_H
#define LWIP_HDR_APPS_SNTP_H

#include "lwip/apps/sntp_opts.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* SNTP operating modes: default is to poll using unicast.
   The mode has to be set before calling sntp_init(). */
#define SNTP_OPMODE_POLL            0
#define SNTP_OPMODE_LISTENONLY      1
void sntp_setoperatingmode(u8_t operating_mode);
u8_t sntp_getoperatingmode(void);

void sntp_init(void);
void sntp_stop(void);
u8_t sntp_enabled(void);

void sntp_setserver(u8_t idx, const ip_addr_t *addr);
const ip_addr_t* sntp_getserver(u8_t idx);

#if SNTP_MONITOR_SERVER_REACHABILITY
u8_t sntp_getreachability(u8_t idx);
#endif /* SNTP_MONITOR_SERVER_REACHABILITY */

#if SNTP_SERVER_DNS
void sntp_setservername(u8_t idx, const char *server);
const char *sntp_getservername(u8_t idx);
#endif /* SNTP_SERVER_DNS */

#if SNTP_GET_SERVERS_FROM_DHCP || SNTP_GET_SERVERS_FROM_DHCPV6
void sntp_servermode_dhcp(int set_servers_from_dhcp);
#else /* SNTP_GET_SERVERS_FROM_DHCP || SNTP_GET_SERVERS_FROM_DHCPV6 */
#define sntp_servermode_dhcp(x)
#endif /* SNTP_GET_SERVERS_FROM_DHCP || SNTP_GET_SERVERS_FROM_DHCPV6 */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_SNTP_H */
