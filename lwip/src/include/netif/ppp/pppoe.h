
#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT && PPPOE_SUPPORT /* don't build if not configured for use in lwipopts.h */

#ifndef PPP_OE_H
#define PPP_OE_H

#include "ppp.h"
#include "lwip/etharp.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct pppoehdr {
  PACK_STRUCT_FLD_8(u8_t vertype);
  PACK_STRUCT_FLD_8(u8_t code);
  PACK_STRUCT_FIELD(u16_t session);
  PACK_STRUCT_FIELD(u16_t plen);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct pppoetag {
  PACK_STRUCT_FIELD(u16_t tag);
  PACK_STRUCT_FIELD(u16_t len);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif


#define PPPOE_STATE_INITIAL   0
#define PPPOE_STATE_PADI_SENT 1
#define PPPOE_STATE_PADR_SENT 2
#define PPPOE_STATE_SESSION   3
/* passive */
#define PPPOE_STATE_PADO_SENT 1

#define PPPOE_HEADERLEN       sizeof(struct pppoehdr)
#define PPPOE_VERTYPE         0x11    /* VER=1, TYPE = 1 */

#define PPPOE_TAG_EOL         0x0000  /* end of list */
#define PPPOE_TAG_SNAME       0x0101  /* service name */
#define PPPOE_TAG_ACNAME      0x0102  /* access concentrator name */
#define PPPOE_TAG_HUNIQUE     0x0103  /* host unique */
#define PPPOE_TAG_ACCOOKIE    0x0104  /* AC cookie */
#define PPPOE_TAG_VENDOR      0x0105  /* vendor specific */
#define PPPOE_TAG_RELAYSID    0x0110  /* relay session id */
#define PPPOE_TAG_SNAME_ERR   0x0201  /* service name error */
#define PPPOE_TAG_ACSYS_ERR   0x0202  /* AC system error */
#define PPPOE_TAG_GENERIC_ERR 0x0203  /* gerneric error */

#define PPPOE_CODE_PADI       0x09    /* Active Discovery Initiation */
#define PPPOE_CODE_PADO       0x07    /* Active Discovery Offer */
#define PPPOE_CODE_PADR       0x19    /* Active Discovery Request */
#define PPPOE_CODE_PADS       0x65    /* Active Discovery Session confirmation */
#define PPPOE_CODE_PADT       0xA7    /* Active Discovery Terminate */

#ifndef PPPOE_MAX_AC_COOKIE_LEN
#define PPPOE_MAX_AC_COOKIE_LEN   64
#endif

struct pppoe_softc {
  struct pppoe_softc *next;
  struct netif *sc_ethif;      /* ethernet interface we are using */
  ppp_pcb *pcb;                /* PPP PCB */

  struct eth_addr sc_dest;     /* hardware address of concentrator */
  u16_t sc_session;            /* PPPoE session id */
  u8_t sc_state;               /* discovery phase or session connected */

#if PPPOE_SCNAME_SUPPORT
  const char *sc_service_name;      /* if != NULL: requested name of service */
  const char *sc_concentrator_name; /* if != NULL: requested concentrator id */
#endif /* PPPOE_SCNAME_SUPPORT */
  u8_t sc_ac_cookie[PPPOE_MAX_AC_COOKIE_LEN]; /* content of AC cookie we must echo back */
  u8_t sc_ac_cookie_len;       /* length of cookie data */
#ifdef PPPOE_SERVER
  u8_t *sc_hunique;            /* content of host unique we must echo back */
  u8_t sc_hunique_len;         /* length of host unique */
#endif
  u8_t sc_padi_retried;        /* number of PADI retries already done */
  u8_t sc_padr_retried;        /* number of PADR retries already done */
};


#define pppoe_init() /* compatibility define, no initialization needed */

ppp_pcb *pppoe_create(struct netif *pppif,
       struct netif *ethif,
       const char *service_name, const char *concentrator_name,
       ppp_link_status_cb_fn link_status_cb, void *ctx_cb);

/*
 * Functions called from lwIP
 * DO NOT CALL FROM lwIP USER APPLICATION.
 */
void pppoe_disc_input(struct netif *netif, struct pbuf *p);
void pppoe_data_input(struct netif *netif, struct pbuf *p);

#ifdef __cplusplus
}
#endif

#endif /* PPP_OE_H */

#endif /* PPP_SUPPORT && PPPOE_SUPPORT */
