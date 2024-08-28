

#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT && ECP_SUPPORT  /* don't build if not configured for use in lwipopts.h */

#ifndef ECP_H
#define ECP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecp_options {
    bool required;		/* Is ECP required? */
    unsigned enctype;		/* Encryption type */
} ecp_options;

extern fsm ecp_fsm[];
extern ecp_options ecp_wantoptions[];
extern ecp_options ecp_gotoptions[];
extern ecp_options ecp_allowoptions[];
extern ecp_options ecp_hisoptions[];

extern const struct protent ecp_protent;

#ifdef __cplusplus
}
#endif

#endif /* ECP_H */
#endif /* PPP_SUPPORT && ECP_SUPPORT */
