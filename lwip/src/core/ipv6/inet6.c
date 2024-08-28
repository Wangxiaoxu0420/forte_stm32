

#include "lwip/opt.h"

#if LWIP_IPV6 && LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */

#include "lwip/def.h"
#include "lwip/inet.h"

/** This variable is initialized by the system to contain the wildcard IPv6 address.
 */
const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;

#endif /* LWIP_IPV6 */
