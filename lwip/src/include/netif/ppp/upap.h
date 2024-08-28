

#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT && PAP_SUPPORT  /* don't build if not configured for use in lwipopts.h */

#ifndef UPAP_H
#define UPAP_H

#include "ppp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Packet header = Code, id, length.
 */
#define UPAP_HEADERLEN	4


/*
 * UPAP codes.
 */
#define UPAP_AUTHREQ	1	/* Authenticate-Request */
#define UPAP_AUTHACK	2	/* Authenticate-Ack */
#define UPAP_AUTHNAK	3	/* Authenticate-Nak */


/*
 * Client states.
 */
#define UPAPCS_INITIAL	0	/* Connection down */
#define UPAPCS_CLOSED	1	/* Connection up, haven't requested auth */
#define UPAPCS_PENDING	2	/* Connection down, have requested auth */
#define UPAPCS_AUTHREQ	3	/* We've sent an Authenticate-Request */
#define UPAPCS_OPEN	4	/* We've received an Ack */
#define UPAPCS_BADAUTH	5	/* We've received a Nak */

/*
 * Server states.
 */
#define UPAPSS_INITIAL	0	/* Connection down */
#define UPAPSS_CLOSED	1	/* Connection up, haven't requested auth */
#define UPAPSS_PENDING	2	/* Connection down, have requested auth */
#define UPAPSS_LISTEN	3	/* Listening for an Authenticate */
#define UPAPSS_OPEN	4	/* We've sent an Ack */
#define UPAPSS_BADAUTH	5	/* We've sent a Nak */


/*
 * Timeouts.
 */
#if 0 /* moved to ppp_opts.h */
#define UPAP_DEFTIMEOUT	3	/* Timeout (seconds) for retransmitting req */
#define UPAP_DEFREQTIME	30	/* Time to wait for auth-req from peer */
#endif /* moved to ppp_opts.h */

/*
 * Each interface is described by upap structure.
 */
#if PAP_SUPPORT
typedef struct upap_state {
    const char *us_user;	/* User */
    u8_t us_userlen;		/* User length */
    const char *us_passwd;	/* Password */
    u8_t us_passwdlen;		/* Password length */
    u8_t us_clientstate;	/* Client state */
#if PPP_SERVER
    u8_t us_serverstate;	/* Server state */
#endif /* PPP_SERVER */
    u8_t us_id;		        /* Current id */
    u8_t us_transmits;		/* Number of auth-reqs sent */
} upap_state;
#endif /* PAP_SUPPORT */


void upap_authwithpeer(ppp_pcb *pcb, const char *user, const char *password);
#if PPP_SERVER
void upap_authpeer(ppp_pcb *pcb);
#endif /* PPP_SERVER */

extern const struct protent pap_protent;

#ifdef __cplusplus
}
#endif

#endif /* UPAP_H */
#endif /* PPP_SUPPORT && PAP_SUPPORT */
