

#ifndef LWIP_HDR_APPS_MDNS_OPTS_H
#define LWIP_HDR_APPS_MDNS_OPTS_H

#include "lwip/opt.h"

/**
 * @defgroup mdns_opts Options
 * @ingroup mdns
 * @{
 */

/**
 * LWIP_MDNS_RESPONDER==1: Turn on multicast DNS module. UDP must be available for MDNS
 * transport. IGMP is needed for IPv4 multicast.
 */
#ifndef LWIP_MDNS_RESPONDER
#define LWIP_MDNS_RESPONDER             0
#endif /* LWIP_MDNS_RESPONDER */

/** The maximum number of services per netif */
#ifndef MDNS_MAX_SERVICES
#define MDNS_MAX_SERVICES               1
#endif

/** MDNS_RESP_USENETIF_EXTCALLBACK==1: register an ext_callback on the netif
 * to automatically restart probing/announcing on status or address change.
 */
#ifndef MDNS_RESP_USENETIF_EXTCALLBACK
#define MDNS_RESP_USENETIF_EXTCALLBACK  LWIP_NETIF_EXT_STATUS_CALLBACK
#endif

/**
 * MDNS_DEBUG: Enable debugging for multicast DNS.
 */
#ifndef MDNS_DEBUG
#define MDNS_DEBUG                       LWIP_DBG_OFF
#endif

/**
 * @}
 */

#endif /* LWIP_HDR_APPS_MDNS_OPTS_H */

