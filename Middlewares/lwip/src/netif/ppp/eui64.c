

#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT && PPP_IPV6_SUPPORT  /* don't build if not configured for use in lwipopts.h */

#include "netif/ppp/ppp_impl.h"
#include "netif/ppp/eui64.h"

/*
 * eui64_ntoa - Make an ascii representation of an interface identifier
 */
char *eui64_ntoa(eui64_t e) {
    static char buf[20];

    sprintf(buf, "%02x%02x:%02x%02x:%02x%02x:%02x%02x",
	     e.e8[0], e.e8[1], e.e8[2], e.e8[3], 
	     e.e8[4], e.e8[5], e.e8[6], e.e8[7]);
    return buf;
}

#endif /* PPP_SUPPORT && PPP_IPV6_SUPPORT */
