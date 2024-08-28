

#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT && MSCHAP_SUPPORT  /* don't build if not configured for use in lwipopts.h */

#ifndef CHAPMS_INCLUDE
#define CHAPMS_INCLUDE

extern const struct chap_digest_type chapms_digest;
extern const struct chap_digest_type chapms2_digest;

#endif /* CHAPMS_INCLUDE */

#endif /* PPP_SUPPORT && MSCHAP_SUPPORT */
