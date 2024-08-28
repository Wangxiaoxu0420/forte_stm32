

#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT && EAP_SUPPORT  /* don't build if not configured for use in lwipopts.h */

#include "netif/ppp/ppp_impl.h"
#include "netif/ppp/eap.h"
#include "netif/ppp/magic.h"
#include "netif/ppp/pppcrypt.h"

#ifdef USE_SRP
#include <t_pwd.h>
#include <t_server.h>
#include <t_client.h>
#endif /* USE_SRP */

#ifndef SHA_DIGESTSIZE
#define	SHA_DIGESTSIZE 20
#endif

#ifdef USE_SRP
static char *pn_secret = NULL;		/* Pseudonym generating secret */
#endif

#if PPP_OPTIONS
/*
 * Command-line options.
 */
static option_t eap_option_list[] = {
    { "eap-restart", o_int, &eap_states[0].es_server.ea_timeout,
      "Set retransmit timeout for EAP Requests (server)" },
    { "eap-max-sreq", o_int, &eap_states[0].es_server.ea_maxrequests,
      "Set max number of EAP Requests sent (server)" },
    { "eap-timeout", o_int, &eap_states[0].es_client.ea_timeout,
      "Set time limit for peer EAP authentication" },
    { "eap-max-rreq", o_int, &eap_states[0].es_client.ea_maxrequests,
      "Set max number of EAP Requests allows (client)" },
    { "eap-interval", o_int, &eap_states[0].es_rechallenge,
      "Set interval for EAP rechallenge" },
#ifdef USE_SRP
    { "srp-interval", o_int, &eap_states[0].es_lwrechallenge,
      "Set interval for SRP lightweight rechallenge" },
    { "srp-pn-secret", o_string, &pn_secret,
      "Long term pseudonym generation secret" },
    { "srp-use-pseudonym", o_bool, &eap_states[0].es_usepseudo,
      "Use pseudonym if offered one by server", 1 },
#endif
    { NULL }
};
#endif /* PPP_OPTIONS */

/*
 * Protocol entry points.
 */
static void eap_init(ppp_pcb *pcb);
static void eap_input(ppp_pcb *pcb, u_char *inp, int inlen);
static void eap_protrej(ppp_pcb *pcb);
static void eap_lowerup(ppp_pcb *pcb);
static void eap_lowerdown(ppp_pcb *pcb);
#if PRINTPKT_SUPPORT
static int  eap_printpkt(const u_char *inp, int inlen,
    void (*)(void *arg, const char *fmt, ...), void *arg);
#endif /* PRINTPKT_SUPPORT */

const struct protent eap_protent = {
	PPP_EAP,		/* protocol number */
	eap_init,		/* initialization procedure */
	eap_input,		/* process a received packet */
	eap_protrej,		/* process a received protocol-reject */
	eap_lowerup,		/* lower layer has gone up */
	eap_lowerdown,		/* lower layer has gone down */
	NULL,			/* open the protocol */
	NULL,			/* close the protocol */
#if PRINTPKT_SUPPORT
	eap_printpkt,		/* print a packet in readable form */
#endif /* PRINTPKT_SUPPORT */
#if PPP_DATAINPUT
	NULL,			/* process a received data packet */
#endif /* PPP_DATAINPUT */
#if PRINTPKT_SUPPORT
	"EAP",			/* text name of protocol */
	NULL,			/* text name of corresponding data protocol */
#endif /* PRINTPKT_SUPPORT */
#if PPP_OPTIONS
	eap_option_list,	/* list of command-line options */
	NULL,			/* check requested options; assign defaults */
#endif /* PPP_OPTIONS */
#if DEMAND_SUPPORT
	NULL,			/* configure interface for demand-dial */
	NULL			/* say whether to bring up link for this pkt */
#endif /* DEMAND_SUPPORT */
};

#ifdef USE_SRP
/*
 * A well-known 2048 bit modulus.
 */
static const u_char wkmodulus[] = {
	0xAC, 0x6B, 0xDB, 0x41, 0x32, 0x4A, 0x9A, 0x9B,
	0xF1, 0x66, 0xDE, 0x5E, 0x13, 0x89, 0x58, 0x2F,
	0xAF, 0x72, 0xB6, 0x65, 0x19, 0x87, 0xEE, 0x07,
	0xFC, 0x31, 0x92, 0x94, 0x3D, 0xB5, 0x60, 0x50,
	0xA3, 0x73, 0x29, 0xCB, 0xB4, 0xA0, 0x99, 0xED,
	0x81, 0x93, 0xE0, 0x75, 0x77, 0x67, 0xA1, 0x3D,
	0xD5, 0x23, 0x12, 0xAB, 0x4B, 0x03, 0x31, 0x0D,
	0xCD, 0x7F, 0x48, 0xA9, 0xDA, 0x04, 0xFD, 0x50,
	0xE8, 0x08, 0x39, 0x69, 0xED, 0xB7, 0x67, 0xB0,
	0xCF, 0x60, 0x95, 0x17, 0x9A, 0x16, 0x3A, 0xB3,
	0x66, 0x1A, 0x05, 0xFB, 0xD5, 0xFA, 0xAA, 0xE8,
	0x29, 0x18, 0xA9, 0x96, 0x2F, 0x0B, 0x93, 0xB8,
	0x55, 0xF9, 0x79, 0x93, 0xEC, 0x97, 0x5E, 0xEA,
	0xA8, 0x0D, 0x74, 0x0A, 0xDB, 0xF4, 0xFF, 0x74,
	0x73, 0x59, 0xD0, 0x41, 0xD5, 0xC3, 0x3E, 0xA7,
	0x1D, 0x28, 0x1E, 0x44, 0x6B, 0x14, 0x77, 0x3B,
	0xCA, 0x97, 0xB4, 0x3A, 0x23, 0xFB, 0x80, 0x16,
	0x76, 0xBD, 0x20, 0x7A, 0x43, 0x6C, 0x64, 0x81,
	0xF1, 0xD2, 0xB9, 0x07, 0x87, 0x17, 0x46, 0x1A,
	0x5B, 0x9D, 0x32, 0xE6, 0x88, 0xF8, 0x77, 0x48,
	0x54, 0x45, 0x23, 0xB5, 0x24, 0xB0, 0xD5, 0x7D,
	0x5E, 0xA7, 0x7A, 0x27, 0x75, 0xD2, 0xEC, 0xFA,
	0x03, 0x2C, 0xFB, 0xDB, 0xF5, 0x2F, 0xB3, 0x78,
	0x61, 0x60, 0x27, 0x90, 0x04, 0xE5, 0x7A, 0xE6,
	0xAF, 0x87, 0x4E, 0x73, 0x03, 0xCE, 0x53, 0x29,
	0x9C, 0xCC, 0x04, 0x1C, 0x7B, 0xC3, 0x08, 0xD8,
	0x2A, 0x56, 0x98, 0xF3, 0xA8, 0xD0, 0xC3, 0x82,
	0x71, 0xAE, 0x35, 0xF8, 0xE9, 0xDB, 0xFB, 0xB6,
	0x94, 0xB5, 0xC8, 0x03, 0xD8, 0x9F, 0x7A, 0xE4,
	0x35, 0xDE, 0x23, 0x6D, 0x52, 0x5F, 0x54, 0x75,
	0x9B, 0x65, 0xE3, 0x72, 0xFC, 0xD6, 0x8E, 0xF2,
	0x0F, 0xA7, 0x11, 0x1F, 0x9E, 0x4A, 0xFF, 0x73
};
#endif

#if PPP_SERVER
/* Local forward declarations. */
static void eap_server_timeout(void *arg);
#endif /* PPP_SERVER */

/*
 * Convert EAP state code to printable string for debug.
 */
static const char * eap_state_name(enum eap_state_code esc)
{
	static const char *state_names[] = { EAP_STATES };

	return (state_names[(int)esc]);
}

/*
 * eap_init - Initialize state for an EAP user.  This is currently
 * called once by main() during start-up.
 */
static void eap_init(ppp_pcb *pcb) {

	BZERO(&pcb->eap, sizeof(eap_state));
#if PPP_SERVER
	pcb->eap.es_server.ea_id = magic();
#endif /* PPP_SERVER */
}

/*
 * eap_client_timeout - Give up waiting for the peer to send any
 * Request messages.
 */
static void eap_client_timeout(void *arg) {
	ppp_pcb *pcb = (ppp_pcb*)arg;

	if (!eap_client_active(pcb))
		return;

	ppp_error("EAP: timeout waiting for Request from peer");
	auth_withpeer_fail(pcb, PPP_EAP);
	pcb->eap.es_client.ea_state = eapBadAuth;
}

/*
 * eap_authwithpeer - Authenticate to our peer (behave as client).
 *
 * Start client state and wait for requests.  This is called only
 * after eap_lowerup.
 */
void eap_authwithpeer(ppp_pcb *pcb, const char *localname) {

	if(NULL == localname)
		return;

	/* Save the peer name we're given */
	pcb->eap.es_client.ea_name = localname;
	pcb->eap.es_client.ea_namelen = strlen(localname);

	pcb->eap.es_client.ea_state = eapListen;

	/*
	 * Start a timer so that if the other end just goes
	 * silent, we don't sit here waiting forever.
	 */
	if (pcb->settings.eap_req_time > 0)
		TIMEOUT(eap_client_timeout, pcb,
		    pcb->settings.eap_req_time);
}

#if PPP_SERVER
/*
 * Format a standard EAP Failure message and send it to the peer.
 * (Server operation)
 */
static void eap_send_failure(ppp_pcb *pcb) {
	struct pbuf *p;
	u_char *outp;

	p = pbuf_alloc(PBUF_RAW, (u16_t)(PPP_HDRLEN + EAP_HEADERLEN), PPP_CTRL_PBUF_TYPE);
	if(NULL == p)
		return;
	if(p->tot_len != p->len) {
		pbuf_free(p);
		return;
	}

	outp = (u_char*)p->payload;

	MAKEHEADER(outp, PPP_EAP);

	PUTCHAR(EAP_FAILURE, outp);
	pcb->eap.es_server.ea_id++;
	PUTCHAR(pcb->eap.es_server.ea_id, outp);
	PUTSHORT(EAP_HEADERLEN, outp);

	ppp_write(pcb, p);

	pcb->eap.es_server.ea_state = eapBadAuth;
	auth_peer_fail(pcb, PPP_EAP);
}

/*
 * Format a standard EAP Success message and send it to the peer.
 * (Server operation)
 */
static void eap_send_success(ppp_pcb *pcb) {
	struct pbuf *p;
	u_char *outp;

	p = pbuf_alloc(PBUF_RAW, (u16_t)(PPP_HDRLEN + EAP_HEADERLEN), PPP_CTRL_PBUF_TYPE);
	if(NULL == p)
		return;
	if(p->tot_len != p->len) {
		pbuf_free(p);
		return;
	}

	outp = (u_char*)p->payload;
    
	MAKEHEADER(outp, PPP_EAP);

	PUTCHAR(EAP_SUCCESS, outp);
	pcb->eap.es_server.ea_id++;
	PUTCHAR(pcb->eap.es_server.ea_id, outp);
	PUTSHORT(EAP_HEADERLEN, outp);

	ppp_write(pcb, p);

	auth_peer_success(pcb, PPP_EAP, 0,
	    pcb->eap.es_server.ea_peer, pcb->eap.es_server.ea_peerlen);
}
#endif /* PPP_SERVER */

#ifdef USE_SRP
/*
 * Set DES key according to pseudonym-generating secret and current
 * date.
 */
static bool
pncrypt_setkey(int timeoffs)
{
	struct tm *tp;
	char tbuf[9];
	SHA1_CTX ctxt;
	u_char dig[SHA_DIGESTSIZE];
	time_t reftime;

	if (pn_secret == NULL)
		return (0);
	reftime = time(NULL) + timeoffs;
	tp = localtime(&reftime);
	SHA1Init(&ctxt);
	SHA1Update(&ctxt, pn_secret, strlen(pn_secret));
	strftime(tbuf, sizeof (tbuf), "%Y%m%d", tp);
	SHA1Update(&ctxt, tbuf, strlen(tbuf));
	SHA1Final(dig, &ctxt);
	/* FIXME: if we want to do SRP, we need to find a way to pass the PolarSSL des_context instead of using static memory */
	return (DesSetkey(dig));
}

static char base64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

struct b64state {
	u32_t bs_bits;
	int bs_offs;
};

static int
b64enc(bs, inp, inlen, outp)
struct b64state *bs;
u_char *inp;
int inlen;
u_char *outp;
{
	int outlen = 0;

	while (inlen > 0) {
		bs->bs_bits = (bs->bs_bits << 8) | *inp++;
		inlen--;
		bs->bs_offs += 8;
		if (bs->bs_offs >= 24) {
			*outp++ = base64[(bs->bs_bits >> 18) & 0x3F];
			*outp++ = base64[(bs->bs_bits >> 12) & 0x3F];
			*outp++ = base64[(bs->bs_bits >> 6) & 0x3F];
			*outp++ = base64[bs->bs_bits & 0x3F];
			outlen += 4;
			bs->bs_offs = 0;
			bs->bs_bits = 0;
		}
	}
	return (outlen);
}

static int
b64flush(bs, outp)
struct b64state *bs;
u_char *outp;
{
	int outlen = 0;

	if (bs->bs_offs == 8) {
		*outp++ = base64[(bs->bs_bits >> 2) & 0x3F];
		*outp++ = base64[(bs->bs_bits << 4) & 0x3F];
		outlen = 2;
	} else if (bs->bs_offs == 16) {
		*outp++ = base64[(bs->bs_bits >> 10) & 0x3F];
		*outp++ = base64[(bs->bs_bits >> 4) & 0x3F];
		*outp++ = base64[(bs->bs_bits << 2) & 0x3F];
		outlen = 3;
	}
	bs->bs_offs = 0;
	bs->bs_bits = 0;
	return (outlen);
}

static int
b64dec(bs, inp, inlen, outp)
struct b64state *bs;
u_char *inp;
int inlen;
u_char *outp;
{
	int outlen = 0;
	char *cp;

	while (inlen > 0) {
		if ((cp = strchr(base64, *inp++)) == NULL)
			break;
		bs->bs_bits = (bs->bs_bits << 6) | (cp - base64);
		inlen--;
		bs->bs_offs += 6;
		if (bs->bs_offs >= 8) {
			*outp++ = bs->bs_bits >> (bs->bs_offs - 8);
			outlen++;
			bs->bs_offs -= 8;
		}
	}
	return (outlen);
}
#endif /* USE_SRP */

#if PPP_SERVER
/*
 * Assume that current waiting server state is complete and figure
 * next state to use based on available authentication data.  'status'
 * indicates if there was an error in handling the last query.  It is
 * 0 for success and non-zero for failure.
 */
static void eap_figure_next_state(ppp_pcb *pcb, int status) {
#ifdef USE_SRP
	unsigned char secbuf[MAXSECRETLEN], clear[8], *sp, *dp;
	struct t_pw tpw;
	struct t_confent *tce, mytce;
	char *cp, *cp2;
	struct t_server *ts;
	int id, i, plen, toffs;
	u_char vals[2];
	struct b64state bs;
#endif /* USE_SRP */

	pcb->settings.eap_timeout_time = pcb->eap.es_savedtime;
	switch (pcb->eap.es_server.ea_state) {
	case eapBadAuth:
		return;

	case eapIdentify:
#ifdef USE_SRP
		/* Discard any previous session. */
		ts = (struct t_server *)pcb->eap.es_server.ea_session;
		if (ts != NULL) {
			t_serverclose(ts);
			pcb->eap.es_server.ea_session = NULL;
			pcb->eap.es_server.ea_skey = NULL;
		}
#endif /* USE_SRP */
		if (status != 0) {
			pcb->eap.es_server.ea_state = eapBadAuth;
			break;
		}
#ifdef USE_SRP
		/* If we've got a pseudonym, try to decode to real name. */
		if (pcb->eap.es_server.ea_peerlen > SRP_PSEUDO_LEN &&
		    strncmp(pcb->eap.es_server.ea_peer, SRP_PSEUDO_ID,
			SRP_PSEUDO_LEN) == 0 &&
		    (pcb->eap.es_server.ea_peerlen - SRP_PSEUDO_LEN) * 3 / 4 <
		    sizeof (secbuf)) {
			BZERO(&bs, sizeof (bs));
			plen = b64dec(&bs,
			    pcb->eap.es_server.ea_peer + SRP_PSEUDO_LEN,
			    pcb->eap.es_server.ea_peerlen - SRP_PSEUDO_LEN,
			    secbuf);
			toffs = 0;
			for (i = 0; i < 5; i++) {
				pncrypt_setkey(toffs);
				toffs -= 86400;
				/* FIXME: if we want to do SRP, we need to find a way to pass the PolarSSL des_context instead of using static memory */
				if (!DesDecrypt(secbuf, clear)) {
					ppp_dbglog("no DES here; cannot decode "
					    "pseudonym");
					return;
				}
				id = *(unsigned char *)clear;
				if (id + 1 <= plen && id + 9 > plen)
					break;
			}
			if (plen % 8 == 0 && i < 5) {
				/*
				 * Note that this is always shorter than the
				 * original stored string, so there's no need
				 * to realloc.
				 */
				if ((i = plen = *(unsigned char *)clear) > 7)
					i = 7;
				pcb->eap.es_server.ea_peerlen = plen;
				dp = (unsigned char *)pcb->eap.es_server.ea_peer;
				MEMCPY(dp, clear + 1, i);
				plen -= i;
				dp += i;
				sp = secbuf + 8;
				while (plen > 0) {
					/* FIXME: if we want to do SRP, we need to find a way to pass the PolarSSL des_context instead of using static memory */
					(void) DesDecrypt(sp, dp);
					sp += 8;
					dp += 8;
					plen -= 8;
				}
				pcb->eap.es_server.ea_peer[
					pcb->eap.es_server.ea_peerlen] = '\0';
				ppp_dbglog("decoded pseudonym to \"%.*q\"",
				    pcb->eap.es_server.ea_peerlen,
				    pcb->eap.es_server.ea_peer);
			} else {
				ppp_dbglog("failed to decode real name");
				/* Stay in eapIdentfy state; requery */
				break;
			}
		}
		/* Look up user in secrets database. */
		if (get_srp_secret(pcb->eap.es_unit, pcb->eap.es_server.ea_peer,
		    pcb->eap.es_server.ea_name, (char *)secbuf, 1) != 0) {
			/* Set up default in case SRP entry is bad */
			pcb->eap.es_server.ea_state = eapMD5Chall;
			/* Get t_confent based on index in srp-secrets */
			id = strtol((char *)secbuf, &cp, 10);
			if (*cp++ != ':' || id < 0)
				break;
			if (id == 0) {
				mytce.index = 0;
				mytce.modulus.data = (u_char *)wkmodulus;
				mytce.modulus.len = sizeof (wkmodulus);
				mytce.generator.data = (u_char *)"\002";
				mytce.generator.len = 1;
				tce = &mytce;
			} else if ((tce = gettcid(id)) != NULL) {
				/*
				 * Client will have to verify this modulus/
				 * generator combination, and that will take
				 * a while.  Lengthen the timeout here.
				 */
				if (pcb->settings.eap_timeout_time > 0 &&
				    pcb->settings.eap_timeout_time < 30)
					pcb->settings.eap_timeout_time = 30;
			} else {
				break;
			}
			if ((cp2 = strchr(cp, ':')) == NULL)
				break;
			*cp2++ = '\0';
			tpw.pebuf.name = pcb->eap.es_server.ea_peer;
			tpw.pebuf.password.len = t_fromb64((char *)tpw.pwbuf,
			    cp);
			tpw.pebuf.password.data = tpw.pwbuf;
			tpw.pebuf.salt.len = t_fromb64((char *)tpw.saltbuf,
			    cp2);
			tpw.pebuf.salt.data = tpw.saltbuf;
			if ((ts = t_serveropenraw(&tpw.pebuf, tce)) == NULL)
				break;
			pcb->eap.es_server.ea_session = (void *)ts;
			pcb->eap.es_server.ea_state = eapSRP1;
			vals[0] = pcb->eap.es_server.ea_id + 1;
			vals[1] = EAPT_SRP;
			t_serveraddexdata(ts, vals, 2);
			/* Generate B; must call before t_servergetkey() */
			t_servergenexp(ts);
			break;
		}
#endif /* USE_SRP */
		pcb->eap.es_server.ea_state = eapMD5Chall;
		break;

	case eapSRP1:
#ifdef USE_SRP
		ts = (struct t_server *)pcb->eap.es_server.ea_session;
		if (ts != NULL && status != 0) {
			t_serverclose(ts);
			pcb->eap.es_server.ea_session = NULL;
			pcb->eap.es_server.ea_skey = NULL;
		}
#endif /* USE_SRP */
		if (status == 1) {
			pcb->eap.es_server.ea_state = eapMD5Chall;
		} else if (status != 0 || pcb->eap.es_server.ea_session == NULL) {
			pcb->eap.es_server.ea_state = eapBadAuth;
		} else {
			pcb->eap.es_server.ea_state = eapSRP2;
		}
		break;

	case eapSRP2:
#ifdef USE_SRP
		ts = (struct t_server *)pcb->eap.es_server.ea_session;
		if (ts != NULL && status != 0) {
			t_serverclose(ts);
			pcb->eap.es_server.ea_session = NULL;
			pcb->eap.es_server.ea_skey = NULL;
		}
#endif /* USE_SRP */
		if (status != 0 || pcb->eap.es_server.ea_session == NULL) {
			pcb->eap.es_server.ea_state = eapBadAuth;
		} else {
			pcb->eap.es_server.ea_state = eapSRP3;
		}
		break;

	case eapSRP3:
	case eapSRP4:
#ifdef USE_SRP
		ts = (struct t_server *)pcb->eap.es_server.ea_session;
		if (ts != NULL && status != 0) {
			t_serverclose(ts);
			pcb->eap.es_server.ea_session = NULL;
			pcb->eap.es_server.ea_skey = NULL;
		}
#endif /* USE_SRP */
		if (status != 0 || pcb->eap.es_server.ea_session == NULL) {
			pcb->eap.es_server.ea_state = eapBadAuth;
		} else {
			pcb->eap.es_server.ea_state = eapOpen;
		}
		break;

	case eapMD5Chall:
		if (status != 0) {
			pcb->eap.es_server.ea_state = eapBadAuth;
		} else {
			pcb->eap.es_server.ea_state = eapOpen;
		}
		break;

	default:
		pcb->eap.es_server.ea_state = eapBadAuth;
		break;
	}
	if (pcb->eap.es_server.ea_state == eapBadAuth)
		eap_send_failure(pcb);
}

/*
 * Format an EAP Request message and send it to the peer.  Message
 * type depends on current state.  (Server operation)
 */
static void eap_send_request(ppp_pcb *pcb) {
	struct pbuf *p;
	u_char *outp;
	u_char *lenloc;
	int outlen;
	int len;
	const char *str;
#ifdef USE_SRP
	struct t_server *ts;
	u_char clear[8], cipher[8], dig[SHA_DIGESTSIZE], *optr, *cp;
	int i, j;
	struct b64state b64;
	SHA1_CTX ctxt;
#endif /* USE_SRP */

	/* Handle both initial auth and restart */
	if (pcb->eap.es_server.ea_state < eapIdentify &&
	    pcb->eap.es_server.ea_state != eapInitial) {
		pcb->eap.es_server.ea_state = eapIdentify;
#if PPP_REMOTENAME
		if (pcb->settings.explicit_remote && pcb->remote_name) {
			/*
			 * If we already know the peer's
			 * unauthenticated name, then there's no
			 * reason to ask.  Go to next state instead.
			 */
			int len = (int)strlen(pcb->remote_name);
			if (len > MAXNAMELEN) {
				len = MAXNAMELEN;
			}
			MEMCPY(pcb->eap.es_server.ea_peer, pcb->remote_name, len);
			pcb->eap.es_server.ea_peer[len] = '\0';
			pcb->eap.es_server.ea_peerlen = len;
			eap_figure_next_state(pcb, 0);
		}
#endif /* PPP_REMOTENAME */
	}

	if (pcb->settings.eap_max_transmits > 0 &&
	    pcb->eap.es_server.ea_requests >= pcb->settings.eap_max_transmits) {
		if (pcb->eap.es_server.ea_responses > 0)
			ppp_error("EAP: too many Requests sent");
		else
			ppp_error("EAP: no response to Requests");
		eap_send_failure(pcb);
		return;
	}

	p = pbuf_alloc(PBUF_RAW, (u16_t)(PPP_CTRL_PBUF_MAX_SIZE), PPP_CTRL_PBUF_TYPE);
	if(NULL == p)
		return;
	if(p->tot_len != p->len) {
		pbuf_free(p);
		return;
	}

	outp = (u_char*)p->payload;
    
	MAKEHEADER(outp, PPP_EAP);

	PUTCHAR(EAP_REQUEST, outp);
	PUTCHAR(pcb->eap.es_server.ea_id, outp);
	lenloc = outp;
	INCPTR(2, outp);

	switch (pcb->eap.es_server.ea_state) {
	case eapIdentify:
		PUTCHAR(EAPT_IDENTITY, outp);
		str = "Name";
		len = strlen(str);
		MEMCPY(outp, str, len);
		INCPTR(len, outp);
		break;

	case eapMD5Chall:
		PUTCHAR(EAPT_MD5CHAP, outp);
		/*
		 * pick a random challenge length between
		 * EAP_MIN_CHALLENGE_LENGTH and EAP_MAX_CHALLENGE_LENGTH
		 */
		pcb->eap.es_challen = EAP_MIN_CHALLENGE_LENGTH +
		    magic_pow(EAP_MIN_MAX_POWER_OF_TWO_CHALLENGE_LENGTH);
		PUTCHAR(pcb->eap.es_challen, outp);
		magic_random_bytes(pcb->eap.es_challenge, pcb->eap.es_challen);
		MEMCPY(outp, pcb->eap.es_challenge, pcb->eap.es_challen);
		INCPTR(pcb->eap.es_challen, outp);
		MEMCPY(outp, pcb->eap.es_server.ea_name, pcb->eap.es_server.ea_namelen);
		INCPTR(pcb->eap.es_server.ea_namelen, outp);
		break;

#ifdef USE_SRP
	case eapSRP1:
		PUTCHAR(EAPT_SRP, outp);
		PUTCHAR(EAPSRP_CHALLENGE, outp);

		PUTCHAR(pcb->eap.es_server.ea_namelen, outp);
		MEMCPY(outp, pcb->eap.es_server.ea_name, pcb->eap.es_server.ea_namelen);
		INCPTR(pcb->eap.es_server.ea_namelen, outp);

		ts = (struct t_server *)pcb->eap.es_server.ea_session;
		assert(ts != NULL);
		PUTCHAR(ts->s.len, outp);
		MEMCPY(outp, ts->s.data, ts->s.len);
		INCPTR(ts->s.len, outp);

		if (ts->g.len == 1 && ts->g.data[0] == 2) {
			PUTCHAR(0, outp);
		} else {
			PUTCHAR(ts->g.len, outp);
			MEMCPY(outp, ts->g.data, ts->g.len);
			INCPTR(ts->g.len, outp);
		}

		if (ts->n.len != sizeof (wkmodulus) ||
		    BCMP(ts->n.data, wkmodulus, sizeof (wkmodulus)) != 0) {
			MEMCPY(outp, ts->n.data, ts->n.len);
			INCPTR(ts->n.len, outp);
		}
		break;

	case eapSRP2:
		PUTCHAR(EAPT_SRP, outp);
		PUTCHAR(EAPSRP_SKEY, outp);

		ts = (struct t_server *)pcb->eap.es_server.ea_session;
		assert(ts != NULL);
		MEMCPY(outp, ts->B.data, ts->B.len);
		INCPTR(ts->B.len, outp);
		break;

	case eapSRP3:
		PUTCHAR(EAPT_SRP, outp);
		PUTCHAR(EAPSRP_SVALIDATOR, outp);
		PUTLONG(SRPVAL_EBIT, outp);
		ts = (struct t_server *)pcb->eap.es_server.ea_session;
		assert(ts != NULL);
		MEMCPY(outp, t_serverresponse(ts), SHA_DIGESTSIZE);
		INCPTR(SHA_DIGESTSIZE, outp);

		if (pncrypt_setkey(0)) {
			/* Generate pseudonym */
			optr = outp;
			cp = (unsigned char *)pcb->eap.es_server.ea_peer;
			if ((j = i = pcb->eap.es_server.ea_peerlen) > 7)
				j = 7;
			clear[0] = i;
			MEMCPY(clear + 1, cp, j);
			i -= j;
			cp += j;
			/* FIXME: if we want to do SRP, we need to find a way to pass the PolarSSL des_context instead of using static memory */
			if (!DesEncrypt(clear, cipher)) {
				ppp_dbglog("no DES here; not generating pseudonym");
				break;
			}
			BZERO(&b64, sizeof (b64));
			outp++;		/* space for pseudonym length */
			outp += b64enc(&b64, cipher, 8, outp);
			while (i >= 8) {
				/* FIXME: if we want to do SRP, we need to find a way to pass the PolarSSL des_context instead of using static memory */
				(void) DesEncrypt(cp, cipher);
				outp += b64enc(&b64, cipher, 8, outp);
				cp += 8;
				i -= 8;
			}
			if (i > 0) {
				MEMCPY(clear, cp, i);
				cp += i;
				magic_random_bytes(cp, 8-i);
				/* FIXME: if we want to do SRP, we need to find a way to pass the PolarSSL des_context instead of using static memory */
				(void) DesEncrypt(clear, cipher);
				outp += b64enc(&b64, cipher, 8, outp);
			}
			outp += b64flush(&b64, outp);

			/* Set length and pad out to next 20 octet boundary */
			i = outp - optr - 1;
			*optr = i;
			i %= SHA_DIGESTSIZE;
			if (i != 0) {
				magic_random_bytes(outp, SHA_DIGESTSIZE-i);
				INCPTR(SHA_DIGESTSIZE-i, outp);
			}

			/* Obscure the pseudonym with SHA1 hash */
			SHA1Init(&ctxt);
			SHA1Update(&ctxt, &pcb->eap.es_server.ea_id, 1);
			SHA1Update(&ctxt, pcb->eap.es_server.ea_skey,
			    SESSION_KEY_LEN);
			SHA1Update(&ctxt, pcb->eap.es_server.ea_peer,
			    pcb->eap.es_server.ea_peerlen);
			while (optr < outp) {
				SHA1Final(dig, &ctxt);
				cp = dig;
				while (cp < dig + SHA_DIGESTSIZE)
					*optr++ ^= *cp++;
				SHA1Init(&ctxt);
				SHA1Update(&ctxt, &pcb->eap.es_server.ea_id, 1);
				SHA1Update(&ctxt, pcb->eap.es_server.ea_skey,
				    SESSION_KEY_LEN);
				SHA1Update(&ctxt, optr - SHA_DIGESTSIZE,
				    SHA_DIGESTSIZE);
			}
		}
		break;

	case eapSRP4:
		PUTCHAR(EAPT_SRP, outp);
		PUTCHAR(EAPSRP_LWRECHALLENGE, outp);
		pcb->eap.es_challen = EAP_MIN_CHALLENGE_LENGTH +
		    magic_pow(EAP_MIN_MAX_POWER_OF_TWO_CHALLENGE_LENGTH);
		magic_random_bytes(pcb->eap.es_challenge, pcb->eap.es_challen);
		MEMCPY(outp, pcb->eap.es_challenge, pcb->eap.es_challen);
		INCPTR(pcb->eap.es_challen, outp);
		break;
#endif /* USE_SRP */

	default:
		return;
	}

	outlen = (outp - (unsigned char*)p->payload) - PPP_HDRLEN;
	PUTSHORT(outlen, lenloc);

	pbuf_realloc(p, outlen + PPP_HDRLEN);
	ppp_write(pcb, p);

	pcb->eap.es_server.ea_requests++;

	if (pcb->settings.eap_timeout_time > 0)
		TIMEOUT(eap_server_timeout, pcb, pcb->settings.eap_timeout_time);
}

/*
 * eap_authpeer - Authenticate our peer (behave as server).
 *
 * Start server state and send first request.  This is called only
 * after eap_lowerup.
 */
void eap_authpeer(ppp_pcb *pcb, const char *localname) {

	/* Save the name we're given. */
	pcb->eap.es_server.ea_name = localname;
	pcb->eap.es_server.ea_namelen = strlen(localname);

	pcb->eap.es_savedtime = pcb->settings.eap_timeout_time;

	/* Lower layer up yet? */
	if (pcb->eap.es_server.ea_state == eapInitial ||
	    pcb->eap.es_server.ea_state == eapPending) {
		pcb->eap.es_server.ea_state = eapPending;
		return;
	}

	pcb->eap.es_server.ea_state = eapPending;

	/* ID number not updated here intentionally; hashed into M1 */
	eap_send_request(pcb);
}

/*
 * eap_server_timeout - Retransmission timer for sending Requests
 * expired.
 */
static void eap_server_timeout(void *arg) {
	ppp_pcb *pcb = (ppp_pcb*)arg;

	if (!eap_server_active(pcb))
		return;

	/* EAP ID number must not change on timeout. */
	eap_send_request(pcb);
}

/*
 * When it's time to send rechallenge the peer, this timeout is
 * called.  Once the rechallenge is successful, the response handler
 * will restart the timer.  If it fails, then the link is dropped.
 */
static void eap_rechallenge(void *arg) {
	ppp_pcb *pcb = (ppp_pcb*)arg;

	if (pcb->eap.es_server.ea_state != eapOpen &&
	    pcb->eap.es_server.ea_state != eapSRP4)
		return;

	pcb->eap.es_server.ea_requests = 0;
	pcb->eap.es_server.ea_state = eapIdentify;
	eap_figure_next_state(pcb, 0);
	pcb->eap.es_server.ea_id++;
	eap_send_request(pcb);
}

static void srp_lwrechallenge(void *arg) {
	ppp_pcb *pcb = (ppp_pcb*)arg;

	if (pcb->eap.es_server.ea_state != eapOpen ||
	    pcb->eap.es_server.ea_type != EAPT_SRP)
		return;

	pcb->eap.es_server.ea_requests = 0;
	pcb->eap.es_server.ea_state = eapSRP4;
	pcb->eap.es_server.ea_id++;
	eap_send_request(pcb);
}
#endif /* PPP_SERVER */

/*
 * eap_lowerup - The lower layer is now up.
 *
 * This is called before either eap_authpeer or eap_authwithpeer.  See
 * link_established() in auth.c.  All that's necessary here is to
 * return to closed state so that those two routines will do the right
 * thing.
 */
static void eap_lowerup(ppp_pcb *pcb) {
	pcb->eap.es_client.ea_state = eapClosed;
#if PPP_SERVER
	pcb->eap.es_server.ea_state = eapClosed;
#endif /* PPP_SERVER */
}

/*
 * eap_lowerdown - The lower layer is now down.
 *
 * Cancel all timeouts and return to initial state.
 */
static void eap_lowerdown(ppp_pcb *pcb) {

	if (eap_client_active(pcb) && pcb->settings.eap_req_time > 0) {
		UNTIMEOUT(eap_client_timeout, pcb);
	}
#if PPP_SERVER
	if (eap_server_active(pcb)) {
		if (pcb->settings.eap_timeout_time > 0) {
			UNTIMEOUT(eap_server_timeout, pcb);
		}
	} else {
		if ((pcb->eap.es_server.ea_state == eapOpen ||
		    pcb->eap.es_server.ea_state == eapSRP4) &&
		    pcb->eap.es_rechallenge > 0) {
			UNTIMEOUT(eap_rechallenge, (void *)pcb);
		}
		if (pcb->eap.es_server.ea_state == eapOpen &&
		    pcb->eap.es_lwrechallenge > 0) {
			UNTIMEOUT(srp_lwrechallenge, (void *)pcb);
		}
	}

	pcb->eap.es_client.ea_state = pcb->eap.es_server.ea_state = eapInitial;
	pcb->eap.es_client.ea_requests = pcb->eap.es_server.ea_requests = 0;
#endif /* PPP_SERVER */
}

/*
 * eap_protrej - Peer doesn't speak this protocol.
 *
 * This shouldn't happen.  If it does, it represents authentication
 * failure.
 */
static void eap_protrej(ppp_pcb *pcb) {

	if (eap_client_active(pcb)) {
		ppp_error("EAP authentication failed due to Protocol-Reject");
		auth_withpeer_fail(pcb, PPP_EAP);
	}
#if PPP_SERVER
	if (eap_server_active(pcb)) {
		ppp_error("EAP authentication of peer failed on Protocol-Reject");
		auth_peer_fail(pcb, PPP_EAP);
	}
#endif /* PPP_SERVER */
	eap_lowerdown(pcb);
}

/*
 * Format and send a regular EAP Response message.
 */
static void eap_send_response(ppp_pcb *pcb, u_char id, u_char typenum, const u_char *str, int lenstr) {
	struct pbuf *p;
	u_char *outp;
	int msglen;

	msglen = EAP_HEADERLEN + sizeof (u_char) + lenstr;
	p = pbuf_alloc(PBUF_RAW, (u16_t)(PPP_HDRLEN + msglen), PPP_CTRL_PBUF_TYPE);
	if(NULL == p)
		return;
	if(p->tot_len != p->len) {
		pbuf_free(p);
		return;
	}

	outp = (u_char*)p->payload;

	MAKEHEADER(outp, PPP_EAP);

	PUTCHAR(EAP_RESPONSE, outp);
	PUTCHAR(id, outp);
	pcb->eap.es_client.ea_id = id;
	PUTSHORT(msglen, outp);
	PUTCHAR(typenum, outp);
	if (lenstr > 0) {
		MEMCPY(outp, str, lenstr);
	}

	ppp_write(pcb, p);
}

/*
 * Format and send an MD5-Challenge EAP Response message.
 */
static void eap_chap_response(ppp_pcb *pcb, u_char id, u_char *hash, const char *name, int namelen) {
	struct pbuf *p;
	u_char *outp;
	int msglen;

	msglen = EAP_HEADERLEN + 2 * sizeof (u_char) + MD5_SIGNATURE_SIZE +
	    namelen;
	p = pbuf_alloc(PBUF_RAW, (u16_t)(PPP_HDRLEN + msglen), PPP_CTRL_PBUF_TYPE);
	if(NULL == p)
		return;
	if(p->tot_len != p->len) {
		pbuf_free(p);
		return;
	}

	outp = (u_char*)p->payload;
    
	MAKEHEADER(outp, PPP_EAP);

	PUTCHAR(EAP_RESPONSE, outp);
	PUTCHAR(id, outp);
	pcb->eap.es_client.ea_id = id;
	PUTSHORT(msglen, outp);
	PUTCHAR(EAPT_MD5CHAP, outp);
	PUTCHAR(MD5_SIGNATURE_SIZE, outp);
	MEMCPY(outp, hash, MD5_SIGNATURE_SIZE);
	INCPTR(MD5_SIGNATURE_SIZE, outp);
	if (namelen > 0) {
		MEMCPY(outp, name, namelen);
	}

	ppp_write(pcb, p);
}

#ifdef USE_SRP
/*
 * Format and send a SRP EAP Response message.
 */
static void
eap_srp_response(esp, id, subtypenum, str, lenstr)
eap_state *esp;
u_char id;
u_char subtypenum;
u_char *str;
int lenstr;
{
	ppp_pcb *pcb = &ppp_pcb_list[pcb->eap.es_unit];
	struct pbuf *p;
	u_char *outp;
	int msglen;

	msglen = EAP_HEADERLEN + 2 * sizeof (u_char) + lenstr;
	p = pbuf_alloc(PBUF_RAW, (u16_t)(PPP_HDRLEN + msglen), PPP_CTRL_PBUF_TYPE);
	if(NULL == p)
		return;
	if(p->tot_len != p->len) {
		pbuf_free(p);
		return;
	}

	outp = p->payload;

	MAKEHEADER(outp, PPP_EAP);

	PUTCHAR(EAP_RESPONSE, outp);
	PUTCHAR(id, outp);
	pcb->eap.es_client.ea_id = id;
	PUTSHORT(msglen, outp);
	PUTCHAR(EAPT_SRP, outp);
	PUTCHAR(subtypenum, outp);
	if (lenstr > 0) {
		MEMCPY(outp, str, lenstr);
	}

	ppp_write(pcb, p);
}

/*
 * Format and send a SRP EAP Client Validator Response message.
 */
static void
eap_srpval_response(esp, id, flags, str)
eap_state *esp;
u_char id;
u32_t flags;
u_char *str;
{
	ppp_pcb *pcb = &ppp_pcb_list[pcb->eap.es_unit];
	struct pbuf *p;
	u_char *outp;
	int msglen;

	msglen = EAP_HEADERLEN + 2 * sizeof (u_char) + sizeof (u32_t) +
	    SHA_DIGESTSIZE;
	p = pbuf_alloc(PBUF_RAW, (u16_t)(PPP_HDRLEN + msglen), PPP_CTRL_PBUF_TYPE);
	if(NULL == p)
		return;
	if(p->tot_len != p->len) {
		pbuf_free(p);
		return;
	}

	outp = p->payload;

	MAKEHEADER(outp, PPP_EAP);

	PUTCHAR(EAP_RESPONSE, outp);
	PUTCHAR(id, outp);
	pcb->eap.es_client.ea_id = id;
	PUTSHORT(msglen, outp);
	PUTCHAR(EAPT_SRP, outp);
	PUTCHAR(EAPSRP_CVALIDATOR, outp);
	PUTLONG(flags, outp);
	MEMCPY(outp, str, SHA_DIGESTSIZE);

	ppp_write(pcb, p);
}
#endif /* USE_SRP */

static void eap_send_nak(ppp_pcb *pcb, u_char id, u_char type) {
	struct pbuf *p;
	u_char *outp;
	int msglen;

	msglen = EAP_HEADERLEN + 2 * sizeof (u_char);
	p = pbuf_alloc(PBUF_RAW, (u16_t)(PPP_HDRLEN + msglen), PPP_CTRL_PBUF_TYPE);
	if(NULL == p)
		return;
	if(p->tot_len != p->len) {
		pbuf_free(p);
		return;
	}

	outp = (u_char*)p->payload;

	MAKEHEADER(outp, PPP_EAP);

	PUTCHAR(EAP_RESPONSE, outp);
	PUTCHAR(id, outp);
	pcb->eap.es_client.ea_id = id;
	PUTSHORT(msglen, outp);
	PUTCHAR(EAPT_NAK, outp);
	PUTCHAR(type, outp);

	ppp_write(pcb, p);
}

#ifdef USE_SRP
static char *
name_of_pn_file()
{
	char *user, *path, *file;
	struct passwd *pw;
	size_t pl;
	static bool pnlogged = 0;

	pw = getpwuid(getuid());
	if (pw == NULL || (user = pw->pw_dir) == NULL || user[0] == 0) {
		errno = EINVAL;
		return (NULL);
	}
	file = _PATH_PSEUDONYM;
	pl = strlen(user) + strlen(file) + 2;
	path = malloc(pl);
	if (path == NULL)
		return (NULL);
	(void) slprintf(path, pl, "%s/%s", user, file);
	if (!pnlogged) {
		ppp_dbglog("pseudonym file: %s", path);
		pnlogged = 1;
	}
	return (path);
}

static int
open_pn_file(modebits)
mode_t modebits;
{
	char *path;
	int fd, err;

	if ((path = name_of_pn_file()) == NULL)
		return (-1);
	fd = open(path, modebits, S_IRUSR | S_IWUSR);
	err = errno;
	free(path);
	errno = err;
	return (fd);
}

static void
remove_pn_file()
{
	char *path;

	if ((path = name_of_pn_file()) != NULL) {
		(void) unlink(path);
		(void) free(path);
	}
}

static void
write_pseudonym(esp, inp, len, id)
eap_state *esp;
u_char *inp;
int len, id;
{
	u_char val;
	u_char *datp, *digp;
	SHA1_CTX ctxt;
	u_char dig[SHA_DIGESTSIZE];
	int dsize, fd, olen = len;

	/*
	 * Do the decoding by working backwards.  This eliminates the need
	 * to save the decoded output in a separate buffer.
	 */
	val = id;
	while (len > 0) {
		if ((dsize = len % SHA_DIGESTSIZE) == 0)
			dsize = SHA_DIGESTSIZE;
		len -= dsize;
		datp = inp + len;
		SHA1Init(&ctxt);
		SHA1Update(&ctxt, &val, 1);
		SHA1Update(&ctxt, pcb->eap.es_client.ea_skey, SESSION_KEY_LEN);
		if (len > 0) {
			SHA1Update(&ctxt, datp, SHA_DIGESTSIZE);
		} else {
			SHA1Update(&ctxt, pcb->eap.es_client.ea_name,
			    pcb->eap.es_client.ea_namelen);
		}
		SHA1Final(dig, &ctxt);
		for (digp = dig; digp < dig + SHA_DIGESTSIZE; digp++)
			*datp++ ^= *digp;
	}

	/* Now check that the result is sane */
	if (olen <= 0 || *inp + 1 > olen) {
		ppp_dbglog("EAP: decoded pseudonym is unusable <%.*B>", olen, inp);
		return;
	}

	/* Save it away */
	fd = open_pn_file(O_WRONLY | O_CREAT | O_TRUNC);
	if (fd < 0) {
		ppp_dbglog("EAP: error saving pseudonym: %m");
		return;
	}
	len = write(fd, inp + 1, *inp);
	if (close(fd) != -1 && len == *inp) {
		ppp_dbglog("EAP: saved pseudonym");
		pcb->eap.es_usedpseudo = 0;
	} else {
		ppp_dbglog("EAP: failed to save pseudonym");
		remove_pn_file();
	}
}
#endif /* USE_SRP */

/*
 * eap_request - Receive EAP Request message (client mode).
 */
static void eap_request(ppp_pcb *pcb, u_char *inp, int id, int len) {
	u_char typenum;
	u_char vallen;
	int secret_len;
	char secret[MAXSECRETLEN];
	char rhostname[MAXNAMELEN];
	lwip_md5_context mdContext;
	u_char hash[MD5_SIGNATURE_SIZE];
#ifdef USE_SRP
	struct t_client *tc;
	struct t_num sval, gval, Nval, *Ap, Bval;
	u_char vals[2];
	SHA1_CTX ctxt;
	u_char dig[SHA_DIGESTSIZE];
	int fd;
#endif /* USE_SRP */

	/*
	 * Note: we update es_client.ea_id *only if* a Response
	 * message is being generated.  Otherwise, we leave it the
	 * same for duplicate detection purposes.
	 */

	pcb->eap.es_client.ea_requests++;
	if (pcb->settings.eap_allow_req != 0 &&
	    pcb->eap.es_client.ea_requests > pcb->settings.eap_allow_req) {
		ppp_info("EAP: received too many Request messages");
		if (pcb->settings.eap_req_time > 0) {
			UNTIMEOUT(eap_client_timeout, pcb);
		}
		auth_withpeer_fail(pcb, PPP_EAP);
		return;
	}

	if (len <= 0) {
		ppp_error("EAP: empty Request message discarded");
		return;
	}

	GETCHAR(typenum, inp);
	len--;

	switch (typenum) {
	case EAPT_IDENTITY:
		if (len > 0)
			ppp_info("EAP: Identity prompt \"%.*q\"", len, inp);
#ifdef USE_SRP
		if (pcb->eap.es_usepseudo &&
		    (pcb->eap.es_usedpseudo == 0 ||
			(pcb->eap.es_usedpseudo == 1 &&
			    id == pcb->eap.es_client.ea_id))) {
			pcb->eap.es_usedpseudo = 1;
			/* Try to get a pseudonym */
			if ((fd = open_pn_file(O_RDONLY)) >= 0) {
				strcpy(rhostname, SRP_PSEUDO_ID);
				len = read(fd, rhostname + SRP_PSEUDO_LEN,
				    sizeof (rhostname) - SRP_PSEUDO_LEN);
				/* XXX NAI unsupported */
				if (len > 0) {
					eap_send_response(pcb, id, typenum,
					    rhostname, len + SRP_PSEUDO_LEN);
				}
				(void) close(fd);
				if (len > 0)
					break;
			}
		}
		/* Stop using pseudonym now. */
		if (pcb->eap.es_usepseudo && pcb->eap.es_usedpseudo != 2) {
			remove_pn_file();
			pcb->eap.es_usedpseudo = 2;
		}
#endif /* USE_SRP */
		eap_send_response(pcb, id, typenum, (const u_char*)pcb->eap.es_client.ea_name,
		    pcb->eap.es_client.ea_namelen);
		break;

	case EAPT_NOTIFICATION:
		if (len > 0)
			ppp_info("EAP: Notification \"%.*q\"", len, inp);
		eap_send_response(pcb, id, typenum, NULL, 0);
		break;

	case EAPT_NAK:
		/*
		 * Avoid the temptation to send Response Nak in reply
		 * to Request Nak here.  It can only lead to trouble.
		 */
		ppp_warn("EAP: unexpected Nak in Request; ignored");
		/* Return because we're waiting for something real. */
		return;

	case EAPT_MD5CHAP:
		if (len < 1) {
			ppp_error("EAP: received MD5-Challenge with no data");
			/* Bogus request; wait for something real. */
			return;
		}
		GETCHAR(vallen, inp);
		len--;
		if (vallen < 8 || vallen > len) {
			ppp_error("EAP: MD5-Challenge with bad length %d (8..%d)",
			    vallen, len);
			/* Try something better. */
			eap_send_nak(pcb, id, EAPT_SRP);
			break;
		}

		/* Not so likely to happen. */
		if (vallen >= len + sizeof (rhostname)) {
			ppp_dbglog("EAP: trimming really long peer name down");
			MEMCPY(rhostname, inp + vallen, sizeof (rhostname) - 1);
			rhostname[sizeof (rhostname) - 1] = '\0';
		} else {
			MEMCPY(rhostname, inp + vallen, len - vallen);
			rhostname[len - vallen] = '\0';
		}

#if PPP_REMOTENAME
		/* In case the remote doesn't give us his name. */
		if (pcb->settings.explicit_remote ||
		    (pcb->settings.remote_name[0] != '\0' && vallen == len))
			strlcpy(rhostname, pcb->settings.remote_name, sizeof (rhostname));
#endif /* PPP_REMOTENAME */

		/*
		 * Get the secret for authenticating ourselves with
		 * the specified host.
		 */
		if (!get_secret(pcb, pcb->eap.es_client.ea_name,
		    rhostname, secret, &secret_len, 0)) {
			ppp_dbglog("EAP: no MD5 secret for auth to %q", rhostname);
			eap_send_nak(pcb, id, EAPT_SRP);
			break;
		}
		lwip_md5_init(&mdContext);
		lwip_md5_starts(&mdContext);
		typenum = id;
		lwip_md5_update(&mdContext, &typenum, 1);
		lwip_md5_update(&mdContext, (u_char *)secret, secret_len);
		BZERO(secret, sizeof (secret));
		lwip_md5_update(&mdContext, inp, vallen);
		lwip_md5_finish(&mdContext, hash);
		lwip_md5_free(&mdContext);
		eap_chap_response(pcb, id, hash, pcb->eap.es_client.ea_name,
		    pcb->eap.es_client.ea_namelen);
		break;

#ifdef USE_SRP
	case EAPT_SRP:
		if (len < 1) {
			ppp_error("EAP: received empty SRP Request");
			/* Bogus request; wait for something real. */
			return;
		}

		/* Get subtype */
		GETCHAR(vallen, inp);
		len--;
		switch (vallen) {
		case EAPSRP_CHALLENGE:
			tc = NULL;
			if (pcb->eap.es_client.ea_session != NULL) {
				tc = (struct t_client *)pcb->eap.es_client.
				    ea_session;
				/*
				 * If this is a new challenge, then start
				 * over with a new client session context.
				 * Otherwise, just resend last response.
				 */
				if (id != pcb->eap.es_client.ea_id) {
					t_clientclose(tc);
					pcb->eap.es_client.ea_session = NULL;
					tc = NULL;
				}
			}
			/* No session key just yet */
			pcb->eap.es_client.ea_skey = NULL;
			if (tc == NULL) {
				int rhostnamelen;

				GETCHAR(vallen, inp);
				len--;
				if (vallen >= len) {
					ppp_error("EAP: badly-formed SRP Challenge"
					    " (name)");
					/* Ignore badly-formed messages */
					return;
				}
				MEMCPY(rhostname, inp, vallen);
				rhostname[vallen] = '\0';
				INCPTR(vallen, inp);
				len -= vallen;

				/*
				 * In case the remote doesn't give us his name,
				 * use configured name.
				 */
				if (explicit_remote ||
				    (remote_name[0] != '\0' && vallen == 0)) {
					strlcpy(rhostname, remote_name,
					    sizeof (rhostname));
				}

				rhostnamelen = (int)strlen(rhostname);
				if (rhostnamelen > MAXNAMELEN) {
					rhostnamelen = MAXNAMELEN;
				}
				MEMCPY(pcb->eap.es_client.ea_peer, rhostname, rhostnamelen);
				pcb->eap.es_client.ea_peer[rhostnamelen] = '\0';
				pcb->eap.es_client.ea_peerlen = rhostnamelen;

				GETCHAR(vallen, inp);
				len--;
				if (vallen >= len) {
					ppp_error("EAP: badly-formed SRP Challenge"
					    " (s)");
					/* Ignore badly-formed messages */
					return;
				}
				sval.data = inp;
				sval.len = vallen;
				INCPTR(vallen, inp);
				len -= vallen;

				GETCHAR(vallen, inp);
				len--;
				if (vallen > len) {
					ppp_error("EAP: badly-formed SRP Challenge"
					    " (g)");
					/* Ignore badly-formed messages */
					return;
				}
				/* If no generator present, then use value 2 */
				if (vallen == 0) {
					gval.data = (u_char *)"\002";
					gval.len = 1;
				} else {
					gval.data = inp;
					gval.len = vallen;
				}
				INCPTR(vallen, inp);
				len -= vallen;

				/*
				 * If no modulus present, then use well-known
				 * value.
				 */
				if (len == 0) {
					Nval.data = (u_char *)wkmodulus;
					Nval.len = sizeof (wkmodulus);
				} else {
					Nval.data = inp;
					Nval.len = len;
				}
				tc = t_clientopen(pcb->eap.es_client.ea_name,
				    &Nval, &gval, &sval);
				if (tc == NULL) {
					eap_send_nak(pcb, id, EAPT_MD5CHAP);
					break;
				}
				pcb->eap.es_client.ea_session = (void *)tc;

				/* Add Challenge ID & type to verifier */
				vals[0] = id;
				vals[1] = EAPT_SRP;
				t_clientaddexdata(tc, vals, 2);
			}
			Ap = t_clientgenexp(tc);
			eap_srp_response(esp, id, EAPSRP_CKEY, Ap->data,
			    Ap->len);
			break;

		case EAPSRP_SKEY:
			tc = (struct t_client *)pcb->eap.es_client.ea_session;
			if (tc == NULL) {
				ppp_warn("EAP: peer sent Subtype 2 without 1");
				eap_send_nak(pcb, id, EAPT_MD5CHAP);
				break;
			}
			if (pcb->eap.es_client.ea_skey != NULL) {
				/*
				 * ID number should not change here.  Warn
				 * if it does (but otherwise ignore).
				 */
				if (id != pcb->eap.es_client.ea_id) {
					ppp_warn("EAP: ID changed from %d to %d "
					    "in SRP Subtype 2 rexmit",
					    pcb->eap.es_client.ea_id, id);
				}
			} else {
				if (get_srp_secret(pcb->eap.es_unit,
				    pcb->eap.es_client.ea_name,
				    pcb->eap.es_client.ea_peer, secret, 0) == 0) {
					/*
					 * Can't work with this peer because
					 * the secret is missing.  Just give
					 * up.
					 */
					eap_send_nak(pcb, id, EAPT_MD5CHAP);
					break;
				}
				Bval.data = inp;
				Bval.len = len;
				t_clientpasswd(tc, secret);
				BZERO(secret, sizeof (secret));
				pcb->eap.es_client.ea_skey =
				    t_clientgetkey(tc, &Bval);
				if (pcb->eap.es_client.ea_skey == NULL) {
					/* Server is rogue; stop now */
					ppp_error("EAP: SRP server is rogue");
					goto client_failure;
				}
			}
			eap_srpval_response(esp, id, SRPVAL_EBIT,
			    t_clientresponse(tc));
			break;

		case EAPSRP_SVALIDATOR:
			tc = (struct t_client *)pcb->eap.es_client.ea_session;
			if (tc == NULL || pcb->eap.es_client.ea_skey == NULL) {
				ppp_warn("EAP: peer sent Subtype 3 without 1/2");
				eap_send_nak(pcb, id, EAPT_MD5CHAP);
				break;
			}
			/*
			 * If we're already open, then this ought to be a
			 * duplicate.  Otherwise, check that the server is
			 * who we think it is.
			 */
			if (pcb->eap.es_client.ea_state == eapOpen) {
				if (id != pcb->eap.es_client.ea_id) {
					ppp_warn("EAP: ID changed from %d to %d "
					    "in SRP Subtype 3 rexmit",
					    pcb->eap.es_client.ea_id, id);
				}
			} else {
				len -= sizeof (u32_t) + SHA_DIGESTSIZE;
				if (len < 0 || t_clientverify(tc, inp +
					sizeof (u32_t)) != 0) {
					ppp_error("EAP: SRP server verification "
					    "failed");
					goto client_failure;
				}
				GETLONG(pcb->eap.es_client.ea_keyflags, inp);
				/* Save pseudonym if user wants it. */
				if (len > 0 && pcb->eap.es_usepseudo) {
					INCPTR(SHA_DIGESTSIZE, inp);
					write_pseudonym(esp, inp, len, id);
				}
			}
			/*
			 * We've verified our peer.  We're now mostly done,
			 * except for waiting on the regular EAP Success
			 * message.
			 */
			eap_srp_response(esp, id, EAPSRP_ACK, NULL, 0);
			break;

		case EAPSRP_LWRECHALLENGE:
			if (len < 4) {
				ppp_warn("EAP: malformed Lightweight rechallenge");
				return;
			}
			SHA1Init(&ctxt);
			vals[0] = id;
			SHA1Update(&ctxt, vals, 1);
			SHA1Update(&ctxt, pcb->eap.es_client.ea_skey,
			    SESSION_KEY_LEN);
			SHA1Update(&ctxt, inp, len);
			SHA1Update(&ctxt, pcb->eap.es_client.ea_name,
			    pcb->eap.es_client.ea_namelen);
			SHA1Final(dig, &ctxt);
			eap_srp_response(esp, id, EAPSRP_LWRECHALLENGE, dig,
			    SHA_DIGESTSIZE);
			break;

		default:
			ppp_error("EAP: unknown SRP Subtype %d", vallen);
			eap_send_nak(pcb, id, EAPT_MD5CHAP);
			break;
		}
		break;
#endif /* USE_SRP */

	default:
		ppp_info("EAP: unknown authentication type %d; Naking", typenum);
		eap_send_nak(pcb, id, EAPT_SRP);
		break;
	}

	if (pcb->settings.eap_req_time > 0) {
		UNTIMEOUT(eap_client_timeout, pcb);
		TIMEOUT(eap_client_timeout, pcb,
		    pcb->settings.eap_req_time);
	}
	return;

#ifdef USE_SRP
client_failure:
	pcb->eap.es_client.ea_state = eapBadAuth;
	if (pcb->settings.eap_req_time > 0) {
		UNTIMEOUT(eap_client_timeout, (void *)esp);
	}
	pcb->eap.es_client.ea_session = NULL;
	t_clientclose(tc);
	auth_withpeer_fail(pcb, PPP_EAP);
#endif /* USE_SRP */
}

#if PPP_SERVER
/*
 * eap_response - Receive EAP Response message (server mode).
 */
static void eap_response(ppp_pcb *pcb, u_char *inp, int id, int len) {
	u_char typenum;
	u_char vallen;
	int secret_len;
	char secret[MAXSECRETLEN];
	char rhostname[MAXNAMELEN];
	lwip_md5_context mdContext;
	u_char hash[MD5_SIGNATURE_SIZE];
#ifdef USE_SRP
	struct t_server *ts;
	struct t_num A;
	SHA1_CTX ctxt;
	u_char dig[SHA_DIGESTSIZE];
#endif /* USE_SRP */

	if (pcb->eap.es_server.ea_id != id) {
		ppp_dbglog("EAP: discarding Response %d; expected ID %d", id,
		    pcb->eap.es_server.ea_id);
		return;
	}

	pcb->eap.es_server.ea_responses++;

	if (len <= 0) {
		ppp_error("EAP: empty Response message discarded");
		return;
	}

	GETCHAR(typenum, inp);
	len--;

	switch (typenum) {
	case EAPT_IDENTITY:
		if (pcb->eap.es_server.ea_state != eapIdentify) {
			ppp_dbglog("EAP discarding unwanted Identify \"%.q\"", len,
			    inp);
			break;
		}
		ppp_info("EAP: unauthenticated peer name \"%.*q\"", len, inp);
		if (len > MAXNAMELEN) {
		  len = MAXNAMELEN;
		}
		MEMCPY(pcb->eap.es_server.ea_peer, inp, len);
		pcb->eap.es_server.ea_peer[len] = '\0';
		pcb->eap.es_server.ea_peerlen = len;
		eap_figure_next_state(pcb, 0);
		break;

	case EAPT_NOTIFICATION:
		ppp_dbglog("EAP unexpected Notification; response discarded");
		break;

	case EAPT_NAK:
		if (len < 1) {
			ppp_info("EAP: Nak Response with no suggested protocol");
			eap_figure_next_state(pcb, 1);
			break;
		}

		GETCHAR(vallen, inp);
		len--;

		if (
#if PPP_REMOTENAME
		!pcb->explicit_remote &&
#endif /* PPP_REMOTENAME */
		pcb->eap.es_server.ea_state == eapIdentify){
			/* Peer cannot Nak Identify Request */
			eap_figure_next_state(pcb, 1);
			break;
		}

		switch (vallen) {
		case EAPT_SRP:
			/* Run through SRP validator selection again. */
			pcb->eap.es_server.ea_state = eapIdentify;
			eap_figure_next_state(pcb, 0);
			break;

		case EAPT_MD5CHAP:
			pcb->eap.es_server.ea_state = eapMD5Chall;
			break;

		default:
			ppp_dbglog("EAP: peer requesting unknown Type %d", vallen);
			switch (pcb->eap.es_server.ea_state) {
			case eapSRP1:
			case eapSRP2:
			case eapSRP3:
				pcb->eap.es_server.ea_state = eapMD5Chall;
				break;
			case eapMD5Chall:
			case eapSRP4:
				pcb->eap.es_server.ea_state = eapIdentify;
				eap_figure_next_state(pcb, 0);
				break;
			default:
				break;
			}
			break;
		}
		break;

	case EAPT_MD5CHAP:
		if (pcb->eap.es_server.ea_state != eapMD5Chall) {
			ppp_error("EAP: unexpected MD5-Response");
			eap_figure_next_state(pcb, 1);
			break;
		}
		if (len < 1) {
			ppp_error("EAP: received MD5-Response with no data");
			eap_figure_next_state(pcb, 1);
			break;
		}
		GETCHAR(vallen, inp);
		len--;
		if (vallen != 16 || vallen > len) {
			ppp_error("EAP: MD5-Response with bad length %d", vallen);
			eap_figure_next_state(pcb, 1);
			break;
		}

		/* Not so likely to happen. */
		if (vallen >= len + sizeof (rhostname)) {
			ppp_dbglog("EAP: trimming really long peer name down");
			MEMCPY(rhostname, inp + vallen, sizeof (rhostname) - 1);
			rhostname[sizeof (rhostname) - 1] = '\0';
		} else {
			MEMCPY(rhostname, inp + vallen, len - vallen);
			rhostname[len - vallen] = '\0';
		}

#if PPP_REMOTENAME
		/* In case the remote doesn't give us his name. */
		if (explicit_remote ||
		    (remote_name[0] != '\0' && vallen == len))
			strlcpy(rhostname, remote_name, sizeof (rhostname));
#endif /* PPP_REMOTENAME */

		/*
		 * Get the secret for authenticating the specified
		 * host.
		 */
		if (!get_secret(pcb, rhostname,
		    pcb->eap.es_server.ea_name, secret, &secret_len, 1)) {
			ppp_dbglog("EAP: no MD5 secret for auth of %q", rhostname);
			eap_send_failure(pcb);
			break;
		}
		lwip_md5_init(&mdContext);
		lwip_md5_starts(&mdContext);
		lwip_md5_update(&mdContext, &pcb->eap.es_server.ea_id, 1);
		lwip_md5_update(&mdContext, (u_char *)secret, secret_len);
		BZERO(secret, sizeof (secret));
		lwip_md5_update(&mdContext, pcb->eap.es_challenge, pcb->eap.es_challen);
		lwip_md5_finish(&mdContext, hash);
		lwip_md5_free(&mdContext);
		if (BCMP(hash, inp, MD5_SIGNATURE_SIZE) != 0) {
			eap_send_failure(pcb);
			break;
		}
		pcb->eap.es_server.ea_type = EAPT_MD5CHAP;
		eap_send_success(pcb);
		eap_figure_next_state(pcb, 0);
		if (pcb->eap.es_rechallenge != 0)
			TIMEOUT(eap_rechallenge, pcb, pcb->eap.es_rechallenge);
		break;

#ifdef USE_SRP
	case EAPT_SRP:
		if (len < 1) {
			ppp_error("EAP: empty SRP Response");
			eap_figure_next_state(pcb, 1);
			break;
		}
		GETCHAR(typenum, inp);
		len--;
		switch (typenum) {
		case EAPSRP_CKEY:
			if (pcb->eap.es_server.ea_state != eapSRP1) {
				ppp_error("EAP: unexpected SRP Subtype 1 Response");
				eap_figure_next_state(pcb, 1);
				break;
			}
			A.data = inp;
			A.len = len;
			ts = (struct t_server *)pcb->eap.es_server.ea_session;
			assert(ts != NULL);
			pcb->eap.es_server.ea_skey = t_servergetkey(ts, &A);
			if (pcb->eap.es_server.ea_skey == NULL) {
				/* Client's A value is bogus; terminate now */
				ppp_error("EAP: bogus A value from client");
				eap_send_failure(pcb);
			} else {
				eap_figure_next_state(pcb, 0);
			}
			break;

		case EAPSRP_CVALIDATOR:
			if (pcb->eap.es_server.ea_state != eapSRP2) {
				ppp_error("EAP: unexpected SRP Subtype 2 Response");
				eap_figure_next_state(pcb, 1);
				break;
			}
			if (len < sizeof (u32_t) + SHA_DIGESTSIZE) {
				ppp_error("EAP: M1 length %d < %d", len,
				    sizeof (u32_t) + SHA_DIGESTSIZE);
				eap_figure_next_state(pcb, 1);
				break;
			}
			GETLONG(pcb->eap.es_server.ea_keyflags, inp);
			ts = (struct t_server *)pcb->eap.es_server.ea_session;
			assert(ts != NULL);
			if (t_serververify(ts, inp)) {
				ppp_info("EAP: unable to validate client identity");
				eap_send_failure(pcb);
				break;
			}
			eap_figure_next_state(pcb, 0);
			break;

		case EAPSRP_ACK:
			if (pcb->eap.es_server.ea_state != eapSRP3) {
				ppp_error("EAP: unexpected SRP Subtype 3 Response");
				eap_send_failure(esp);
				break;
			}
			pcb->eap.es_server.ea_type = EAPT_SRP;
			eap_send_success(pcb, esp);
			eap_figure_next_state(pcb, 0);
			if (pcb->eap.es_rechallenge != 0)
				TIMEOUT(eap_rechallenge, pcb,
				    pcb->eap.es_rechallenge);
			if (pcb->eap.es_lwrechallenge != 0)
				TIMEOUT(srp_lwrechallenge, pcb,
				    pcb->eap.es_lwrechallenge);
			break;

		case EAPSRP_LWRECHALLENGE:
			if (pcb->eap.es_server.ea_state != eapSRP4) {
				ppp_info("EAP: unexpected SRP Subtype 4 Response");
				return;
			}
			if (len != SHA_DIGESTSIZE) {
				ppp_error("EAP: bad Lightweight rechallenge "
				    "response");
				return;
			}
			SHA1Init(&ctxt);
			vallen = id;
			SHA1Update(&ctxt, &vallen, 1);
			SHA1Update(&ctxt, pcb->eap.es_server.ea_skey,
			    SESSION_KEY_LEN);
			SHA1Update(&ctxt, pcb->eap.es_challenge, pcb->eap.es_challen);
			SHA1Update(&ctxt, pcb->eap.es_server.ea_peer,
			    pcb->eap.es_server.ea_peerlen);
			SHA1Final(dig, &ctxt);
			if (BCMP(dig, inp, SHA_DIGESTSIZE) != 0) {
				ppp_error("EAP: failed Lightweight rechallenge");
				eap_send_failure(pcb);
				break;
			}
			pcb->eap.es_server.ea_state = eapOpen;
			if (pcb->eap.es_lwrechallenge != 0)
				TIMEOUT(srp_lwrechallenge, esp,
				    pcb->eap.es_lwrechallenge);
			break;
		}
		break;
#endif /* USE_SRP */

	default:
		/* This can't happen. */
		ppp_error("EAP: unknown Response type %d; ignored", typenum);
		return;
	}

	if (pcb->settings.eap_timeout_time > 0) {
		UNTIMEOUT(eap_server_timeout, pcb);
	}

	if (pcb->eap.es_server.ea_state != eapBadAuth &&
	    pcb->eap.es_server.ea_state != eapOpen) {
		pcb->eap.es_server.ea_id++;
		eap_send_request(pcb);
	}
}
#endif /* PPP_SERVER */

/*
 * eap_success - Receive EAP Success message (client mode).
 */
static void eap_success(ppp_pcb *pcb, u_char *inp, int id, int len) {
	LWIP_UNUSED_ARG(id);

	if (pcb->eap.es_client.ea_state != eapOpen && !eap_client_active(pcb)) {
		ppp_dbglog("EAP unexpected success message in state %s (%d)",
		    eap_state_name(pcb->eap.es_client.ea_state),
		    pcb->eap.es_client.ea_state);
		return;
	}

	if (pcb->settings.eap_req_time > 0) {
		UNTIMEOUT(eap_client_timeout, pcb);
	}

	if (len > 0) {
		/* This is odd.  The spec doesn't allow for this. */
		PRINTMSG(inp, len);
	}

	pcb->eap.es_client.ea_state = eapOpen;
	auth_withpeer_success(pcb, PPP_EAP, 0);
}

/*
 * eap_failure - Receive EAP Failure message (client mode).
 */
static void eap_failure(ppp_pcb *pcb, u_char *inp, int id, int len) {
	LWIP_UNUSED_ARG(id);

	if (!eap_client_active(pcb)) {
		ppp_dbglog("EAP unexpected failure message in state %s (%d)",
		    eap_state_name(pcb->eap.es_client.ea_state),
		    pcb->eap.es_client.ea_state);
	}

	if (pcb->settings.eap_req_time > 0) {
		UNTIMEOUT(eap_client_timeout, pcb);
	}

	if (len > 0) {
		/* This is odd.  The spec doesn't allow for this. */
		PRINTMSG(inp, len);
	}

	pcb->eap.es_client.ea_state = eapBadAuth;

	ppp_error("EAP: peer reports authentication failure");
	auth_withpeer_fail(pcb, PPP_EAP);
}

/*
 * eap_input - Handle received EAP message.
 */
static void eap_input(ppp_pcb *pcb, u_char *inp, int inlen) {
	u_char code, id;
	int len;

	/*
	 * Parse header (code, id and length).  If packet too short,
	 * drop it.
	 */
	if (inlen < EAP_HEADERLEN) {
		ppp_error("EAP: packet too short: %d < %d", inlen, EAP_HEADERLEN);
		return;
	}
	GETCHAR(code, inp);
	GETCHAR(id, inp);
	GETSHORT(len, inp);
	if (len < EAP_HEADERLEN || len > inlen) {
		ppp_error("EAP: packet has illegal length field %d (%d..%d)", len,
		    EAP_HEADERLEN, inlen);
		return;
	}
	len -= EAP_HEADERLEN;

	/* Dispatch based on message code */
	switch (code) {
	case EAP_REQUEST:
		eap_request(pcb, inp, id, len);
		break;

#if PPP_SERVER
	case EAP_RESPONSE:
		eap_response(pcb, inp, id, len);
		break;
#endif /* PPP_SERVER */

	case EAP_SUCCESS:
		eap_success(pcb, inp, id, len);
		break;

	case EAP_FAILURE:
		eap_failure(pcb, inp, id, len);
		break;

	default:				/* XXX Need code reject */
		/* Note: it's not legal to send EAP Nak here. */
		ppp_warn("EAP: unknown code %d received", code);
		break;
	}
}

#if PRINTPKT_SUPPORT
/*
 * eap_printpkt - print the contents of an EAP packet.
 */
static const char* const eap_codenames[] = {
	"Request", "Response", "Success", "Failure"
};

static const char* const eap_typenames[] = {
	"Identity", "Notification", "Nak", "MD5-Challenge",
	"OTP", "Generic-Token", NULL, NULL,
	"RSA", "DSS", "KEA", "KEA-Validate",
	"TLS", "Defender", "Windows 2000", "Arcot",
	"Cisco", "Nokia", "SRP"
};

static int eap_printpkt(const u_char *inp, int inlen, void (*printer) (void *, const char *, ...), void *arg) {
	int code, id, len, rtype, vallen;
	const u_char *pstart;
	u32_t uval;

	if (inlen < EAP_HEADERLEN)
		return (0);
	pstart = inp;
	GETCHAR(code, inp);
	GETCHAR(id, inp);
	GETSHORT(len, inp);
	if (len < EAP_HEADERLEN || len > inlen)
		return (0);

	if (code >= 1 && code <= (int)LWIP_ARRAYSIZE(eap_codenames))
		printer(arg, " %s", eap_codenames[code-1]);
	else
		printer(arg, " code=0x%x", code);
	printer(arg, " id=0x%x", id);
	len -= EAP_HEADERLEN;
	switch (code) {
	case EAP_REQUEST:
		if (len < 1) {
			printer(arg, " <missing type>");
			break;
		}
		GETCHAR(rtype, inp);
		len--;
		if (rtype >= 1 && rtype <= (int)LWIP_ARRAYSIZE(eap_typenames))
			printer(arg, " %s", eap_typenames[rtype-1]);
		else
			printer(arg, " type=0x%x", rtype);
		switch (rtype) {
		case EAPT_IDENTITY:
		case EAPT_NOTIFICATION:
			if (len > 0) {
				printer(arg, " <Message ");
				ppp_print_string(inp, len, printer, arg);
				printer(arg, ">");
				INCPTR(len, inp);
				len = 0;
			} else {
				printer(arg, " <No message>");
			}
			break;

		case EAPT_MD5CHAP:
			if (len <= 0)
				break;
			GETCHAR(vallen, inp);
			len--;
			if (vallen > len)
				goto truncated;
			printer(arg, " <Value%.*B>", vallen, inp);
			INCPTR(vallen, inp);
			len -= vallen;
			if (len > 0) {
				printer(arg, " <Name ");
				ppp_print_string(inp, len, printer, arg);
				printer(arg, ">");
				INCPTR(len, inp);
				len = 0;
			} else {
				printer(arg, " <No name>");
			}
			break;

		case EAPT_SRP:
			if (len < 3)
				goto truncated;
			GETCHAR(vallen, inp);
			len--;
			printer(arg, "-%d", vallen);
			switch (vallen) {
			case EAPSRP_CHALLENGE:
				GETCHAR(vallen, inp);
				len--;
				if (vallen >= len)
					goto truncated;
				if (vallen > 0) {
					printer(arg, " <Name ");
					ppp_print_string(inp, vallen, printer,
					    arg);
					printer(arg, ">");
				} else {
					printer(arg, " <No name>");
				}
				INCPTR(vallen, inp);
				len -= vallen;
				GETCHAR(vallen, inp);
				len--;
				if (vallen >= len)
					goto truncated;
				printer(arg, " <s%.*B>", vallen, inp);
				INCPTR(vallen, inp);
				len -= vallen;
				GETCHAR(vallen, inp);
				len--;
				if (vallen > len)
					goto truncated;
				if (vallen == 0) {
					printer(arg, " <Default g=2>");
				} else {
					printer(arg, " <g%.*B>", vallen, inp);
				}
				INCPTR(vallen, inp);
				len -= vallen;
				if (len == 0) {
					printer(arg, " <Default N>");
				} else {
					printer(arg, " <N%.*B>", len, inp);
					INCPTR(len, inp);
					len = 0;
				}
				break;

			case EAPSRP_SKEY:
				printer(arg, " <B%.*B>", len, inp);
				INCPTR(len, inp);
				len = 0;
				break;

			case EAPSRP_SVALIDATOR:
				if (len < (int)sizeof (u32_t))
					break;
				GETLONG(uval, inp);
				len -= sizeof (u32_t);
				if (uval & SRPVAL_EBIT) {
					printer(arg, " E");
					uval &= ~SRPVAL_EBIT;
				}
				if (uval != 0) {
					printer(arg, " f<%X>", uval);
				}
				if ((vallen = len) > SHA_DIGESTSIZE)
					vallen = SHA_DIGESTSIZE;
				printer(arg, " <M2%.*B%s>", len, inp,
				    len < SHA_DIGESTSIZE ? "?" : "");
				INCPTR(vallen, inp);
				len -= vallen;
				if (len > 0) {
					printer(arg, " <PN%.*B>", len, inp);
					INCPTR(len, inp);
					len = 0;
				}
				break;

			case EAPSRP_LWRECHALLENGE:
				printer(arg, " <Challenge%.*B>", len, inp);
				INCPTR(len, inp);
				len = 0;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		break;

	case EAP_RESPONSE:
		if (len < 1)
			break;
		GETCHAR(rtype, inp);
		len--;
		if (rtype >= 1 && rtype <= (int)LWIP_ARRAYSIZE(eap_typenames))
			printer(arg, " %s", eap_typenames[rtype-1]);
		else
			printer(arg, " type=0x%x", rtype);
		switch (rtype) {
		case EAPT_IDENTITY:
			if (len > 0) {
				printer(arg, " <Name ");
				ppp_print_string(inp, len, printer, arg);
				printer(arg, ">");
				INCPTR(len, inp);
				len = 0;
			}
			break;

		case EAPT_NAK:
			if (len <= 0) {
				printer(arg, " <missing hint>");
				break;
			}
			GETCHAR(rtype, inp);
			len--;
			printer(arg, " <Suggested-type %02X", rtype);
			if (rtype >= 1 && rtype < (int)LWIP_ARRAYSIZE(eap_typenames))
				printer(arg, " (%s)", eap_typenames[rtype-1]);
			printer(arg, ">");
			break;

		case EAPT_MD5CHAP:
			if (len <= 0) {
				printer(arg, " <missing length>");
				break;
			}
			GETCHAR(vallen, inp);
			len--;
			if (vallen > len)
				goto truncated;
			printer(arg, " <Value%.*B>", vallen, inp);
			INCPTR(vallen, inp);
			len -= vallen;
			if (len > 0) {
				printer(arg, " <Name ");
				ppp_print_string(inp, len, printer, arg);
				printer(arg, ">");
				INCPTR(len, inp);
				len = 0;
			} else {
				printer(arg, " <No name>");
			}
			break;

		case EAPT_SRP:
			if (len < 1)
				goto truncated;
			GETCHAR(vallen, inp);
			len--;
			printer(arg, "-%d", vallen);
			switch (vallen) {
			case EAPSRP_CKEY:
				printer(arg, " <A%.*B>", len, inp);
				INCPTR(len, inp);
				len = 0;
				break;

			case EAPSRP_CVALIDATOR:
				if (len < (int)sizeof (u32_t))
					break;
				GETLONG(uval, inp);
				len -= sizeof (u32_t);
				if (uval & SRPVAL_EBIT) {
					printer(arg, " E");
					uval &= ~SRPVAL_EBIT;
				}
				if (uval != 0) {
					printer(arg, " f<%X>", uval);
				}
				printer(arg, " <M1%.*B%s>", len, inp,
				    len == SHA_DIGESTSIZE ? "" : "?");
				INCPTR(len, inp);
				len = 0;
				break;

			case EAPSRP_ACK:
				break;

			case EAPSRP_LWRECHALLENGE:
				printer(arg, " <Response%.*B%s>", len, inp,
				    len == SHA_DIGESTSIZE ? "" : "?");
				if ((vallen = len) > SHA_DIGESTSIZE)
					vallen = SHA_DIGESTSIZE;
				INCPTR(vallen, inp);
				len -= vallen;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		break;

	case EAP_SUCCESS:	/* No payload expected for these! */
	case EAP_FAILURE:
	default:
		break;

	truncated:
		printer(arg, " <truncated>");
		break;
	}

	if (len > 8)
		printer(arg, "%8B...", inp);
	else if (len > 0)
		printer(arg, "%.*B", len, inp);
	INCPTR(len, inp);

	return (inp - pstart);
}
#endif /* PRINTPKT_SUPPORT */

#endif /* PPP_SUPPORT && EAP_SUPPORT */
