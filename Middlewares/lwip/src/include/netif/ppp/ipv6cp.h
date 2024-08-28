

#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT && PPP_IPV6_SUPPORT  /* don't build if not configured for use in lwipopts.h */

#ifndef IPV6CP_H
#define	IPV6CP_H

#include "eui64.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Options.
 */
#define CI_IFACEID	1	/* Interface Identifier */
#ifdef IPV6CP_COMP
#define CI_COMPRESSTYPE	2	/* Compression Type     */
#endif /* IPV6CP_COMP */

/* No compression types yet defined.
 *#define IPV6CP_COMP	0x004f
 */
typedef struct ipv6cp_options {
    unsigned int neg_ifaceid    :1;  /* Negotiate interface identifier? */
    unsigned int req_ifaceid    :1;  /* Ask peer to send interface identifier? */
    unsigned int accept_local   :1;  /* accept peer's value for iface id? */
    unsigned int opt_local      :1;  /* ourtoken set by option */
    unsigned int opt_remote     :1;  /* histoken set by option */
    unsigned int use_ip         :1;  /* use IP as interface identifier */
#if 0
    unsigned int use_persistent :1;  /* use uniquely persistent value for address */
#endif
#ifdef IPV6CP_COMP
    unsigned int neg_vj         :1;  /* Van Jacobson Compression? */
#endif /* IPV6CP_COMP */

#ifdef IPV6CP_COMP
    u_short vj_protocol;        /* protocol value to use in VJ option */
#endif /* IPV6CP_COMP */
    eui64_t ourid, hisid;       /* Interface identifiers */
} ipv6cp_options;

extern const struct protent ipv6cp_protent;

#ifdef __cplusplus
}
#endif

#endif /* IPV6CP_H */
#endif /* PPP_SUPPORT && PPP_IPV6_SUPPORT */
