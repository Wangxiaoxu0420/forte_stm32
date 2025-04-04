

#ifndef LWIP_PPPAPI_H
#define LWIP_PPPAPI_H

#include "netif/ppp/ppp_opts.h"

#if LWIP_PPP_API /* don't build if not configured for use in lwipopts.h */

#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/priv/tcpip_priv.h"
#include "netif/ppp/ppp.h"
#if PPPOS_SUPPORT
#include "netif/ppp/pppos.h"
#endif /* PPPOS_SUPPORT */

#ifdef __cplusplus
extern "C" {
#endif

struct pppapi_msg_msg {
  ppp_pcb *ppp;
  union {
#if PPP_NOTIFY_PHASE
    struct {
      ppp_notify_phase_cb_fn notify_phase_cb;
    } setnotifyphasecb;
#endif /* PPP_NOTIFY_PHASE */
#if PPPOS_SUPPORT
    struct {
      struct netif *pppif;
      pppos_output_cb_fn output_cb;
      ppp_link_status_cb_fn link_status_cb;
      void *ctx_cb;
    } serialcreate;
#endif /* PPPOS_SUPPORT */
#if PPPOE_SUPPORT
    struct {
      struct netif *pppif;
      struct netif *ethif;
      const char *service_name;
      const char *concentrator_name;
      ppp_link_status_cb_fn link_status_cb;
      void *ctx_cb;
    } ethernetcreate;
#endif /* PPPOE_SUPPORT */
#if PPPOL2TP_SUPPORT
    struct {
      struct netif *pppif;
      struct netif *netif;
      API_MSG_M_DEF_C(ip_addr_t, ipaddr);
      u16_t port;
#if PPPOL2TP_AUTH_SUPPORT
      const u8_t *secret;
      u8_t secret_len;
#endif /* PPPOL2TP_AUTH_SUPPORT */
      ppp_link_status_cb_fn link_status_cb;
      void *ctx_cb;
    } l2tpcreate;
#endif /* PPPOL2TP_SUPPORT */
    struct {
      u16_t holdoff;
    } connect;
    struct {
      u8_t nocarrier;
    } close;
    struct {
      u8_t cmd;
      void *arg;
    } ioctl;
  } msg;
};

struct pppapi_msg {
  struct tcpip_api_call_data call;
  struct pppapi_msg_msg msg;
};

/* API for application */
err_t pppapi_set_default(ppp_pcb *pcb);
#if PPP_NOTIFY_PHASE
err_t pppapi_set_notify_phase_callback(ppp_pcb *pcb, ppp_notify_phase_cb_fn notify_phase_cb);
#endif /* PPP_NOTIFY_PHASE */
#if PPPOS_SUPPORT
ppp_pcb *pppapi_pppos_create(struct netif *pppif, pppos_output_cb_fn output_cb, ppp_link_status_cb_fn link_status_cb, void *ctx_cb);
#endif /* PPPOS_SUPPORT */
#if PPPOE_SUPPORT
ppp_pcb *pppapi_pppoe_create(struct netif *pppif, struct netif *ethif, const char *service_name,
                                const char *concentrator_name, ppp_link_status_cb_fn link_status_cb,
                                void *ctx_cb);
#endif /* PPPOE_SUPPORT */
#if PPPOL2TP_SUPPORT
ppp_pcb *pppapi_pppol2tp_create(struct netif *pppif, struct netif *netif, ip_addr_t *ipaddr, u16_t port,
                            const u8_t *secret, u8_t secret_len,
                            ppp_link_status_cb_fn link_status_cb, void *ctx_cb);
#endif /* PPPOL2TP_SUPPORT */
err_t pppapi_connect(ppp_pcb *pcb, u16_t holdoff);
#if PPP_SERVER
err_t pppapi_listen(ppp_pcb *pcb);
#endif /* PPP_SERVER */
err_t pppapi_close(ppp_pcb *pcb, u8_t nocarrier);
err_t pppapi_free(ppp_pcb *pcb);
err_t pppapi_ioctl(ppp_pcb *pcb, u8_t cmd, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_PPP_API */

#endif /* LWIP_PPPAPI_H */
