
#ifndef LWIP_HDR_APPS_NETBIOS_OPTS_H
#define LWIP_HDR_APPS_NETBIOS_OPTS_H

#include "lwip/opt.h"

/**
 * @defgroup netbiosns_opts Options
 * @ingroup netbiosns
 * @{
 */

/** NetBIOS name of lwip device
 * This must be uppercase until NETBIOS_STRCMP() is defined to a string
 * comparision function that is case insensitive.
 * If you want to use the netif's hostname, use this (with LWIP_NETIF_HOSTNAME):
 * (ip_current_netif() != NULL ? ip_current_netif()->hostname != NULL ? ip_current_netif()->hostname : "" : "")
 *
 * If this is not defined, netbiosns_set_name() can be called at runtime to change the name.
 */
#ifdef __DOXYGEN__
#define NETBIOS_LWIP_NAME "NETBIOSLWIPDEV"
#endif

/** Respond to NetBIOS name queries
 * Default is disabled
 */
#if !defined LWIP_NETBIOS_RESPOND_NAME_QUERY || defined __DOXYGEN__
#define LWIP_NETBIOS_RESPOND_NAME_QUERY   0
#endif

/**
 * @}
 */

#endif /* LWIP_HDR_APPS_NETBIOS_OPTS_H */
