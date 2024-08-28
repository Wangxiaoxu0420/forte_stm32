

#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT && ECP_SUPPORT  /* don't build if not configured for use in lwipopts.h */

#include <string.h>

#include "netif/ppp/ppp_impl.h"

#include "netif/ppp/fsm.h"
#include "netif/ppp/ecp.h"

#if PPP_OPTIONS
static option_t ecp_option_list[] = {
    { "noecp", o_bool, &ecp_protent.enabled_flag,
      "Disable ECP negotiation" },
    { "-ecp", o_bool, &ecp_protent.enabled_flag,
      "Disable ECP negotiation", OPT_ALIAS },

    { NULL }
};
#endif /* PPP_OPTIONS */

/*
 * Protocol entry points from main code.
 */
static void ecp_init (int unit);
/*
static void ecp_open (int unit);
static void ecp_close (int unit, char *);
static void ecp_lowerup (int unit);
static void ecp_lowerdown (int);
static void ecp_input (int unit, u_char *pkt, int len);
static void ecp_protrej (int unit);
*/
#if PRINTPKT_SUPPORT
static int  ecp_printpkt (const u_char *pkt, int len,
			      void (*printer) (void *, char *, ...),
			      void *arg);
#endif /* PRINTPKT_SUPPORT */
/*
static void ecp_datainput (int unit, u_char *pkt, int len);
*/

const struct protent ecp_protent = {
    PPP_ECP,
    ecp_init,
    NULL, /* ecp_input, */
    NULL, /* ecp_protrej, */
    NULL, /* ecp_lowerup, */
    NULL, /* ecp_lowerdown, */
    NULL, /* ecp_open, */
    NULL, /* ecp_close, */
#if PRINTPKT_SUPPORT
    ecp_printpkt,
#endif /* PRINTPKT_SUPPORT */
#if PPP_DATAINPUT
    NULL, /* ecp_datainput, */
#endif /* PPP_DATAINPUT */
#if PRINTPKT_SUPPORT
    "ECP",
    "Encrypted",
#endif /* PRINTPKT_SUPPORT */
#if PPP_OPTIONS
    ecp_option_list,
    NULL,
#endif /* PPP_OPTIONS */
#if DEMAND_SUPPORT
    NULL,
    NULL
#endif /* DEMAND_SUPPORT */
};

fsm ecp_fsm[NUM_PPP];
ecp_options ecp_wantoptions[NUM_PPP];	/* what to request the peer to use */
ecp_options ecp_gotoptions[NUM_PPP];	/* what the peer agreed to do */
ecp_options ecp_allowoptions[NUM_PPP];	/* what we'll agree to do */
ecp_options ecp_hisoptions[NUM_PPP];	/* what we agreed to do */

static const fsm_callbacks ecp_callbacks = {
    NULL, /* ecp_resetci, */
    NULL, /* ecp_cilen, */
    NULL, /* ecp_addci, */
    NULL, /* ecp_ackci, */
    NULL, /* ecp_nakci, */
    NULL, /* ecp_rejci, */
    NULL, /* ecp_reqci, */
    NULL, /* ecp_up, */
    NULL, /* ecp_down, */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL, /* ecp_extcode, */
    "ECP"
};

/*
 * ecp_init - initialize ECP.
 */
static void
ecp_init(unit)
    int unit;
{
    fsm *f = &ecp_fsm[unit];

    f->unit = unit;
    f->protocol = PPP_ECP;
    f->callbacks = &ecp_callbacks;
    fsm_init(f);

#if 0 /* Not necessary, everything is cleared in ppp_new() */
    memset(&ecp_wantoptions[unit],  0, sizeof(ecp_options));
    memset(&ecp_gotoptions[unit],   0, sizeof(ecp_options));
    memset(&ecp_allowoptions[unit], 0, sizeof(ecp_options));
    memset(&ecp_hisoptions[unit],   0, sizeof(ecp_options));
#endif /* 0 */

}


#if PRINTPKT_SUPPORT
static int
ecp_printpkt(p, plen, printer, arg)
    const u_char *p;
    int plen;
    void (*printer) (void *, char *, ...);
    void *arg;
{
    return 0;
}
#endif /* PRINTPKT_SUPPORT */

#endif /* PPP_SUPPORT && ECP_SUPPORT */
