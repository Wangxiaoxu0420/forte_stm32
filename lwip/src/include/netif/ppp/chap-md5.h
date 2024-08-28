

#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT && CHAP_SUPPORT  /* don't build if not configured for use in lwipopts.h */

extern const struct chap_digest_type md5_digest;

#endif /* PPP_SUPPORT && CHAP_SUPPORT */
