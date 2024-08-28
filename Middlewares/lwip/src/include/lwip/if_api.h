
#ifndef LWIP_HDR_IF_H
#define LWIP_HDR_IF_H

#include "lwip/opt.h"

#if LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */

#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IF_NAMESIZE
#define IF_NAMESIZE NETIF_NAMESIZE
#endif

char * lwip_if_indextoname(unsigned int ifindex, char *ifname);
unsigned int lwip_if_nametoindex(const char *ifname);

#if LWIP_COMPAT_SOCKETS
#define if_indextoname(ifindex, ifname)  lwip_if_indextoname(ifindex,ifname)
#define if_nametoindex(ifname)           lwip_if_nametoindex(ifname)
#endif /* LWIP_COMPAT_SOCKETS */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_SOCKET */

#endif /* LWIP_HDR_IF_H */
