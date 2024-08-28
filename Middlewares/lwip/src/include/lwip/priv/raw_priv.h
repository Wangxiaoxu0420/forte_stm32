
#ifndef LWIP_HDR_RAW_PRIV_H
#define LWIP_HDR_RAW_PRIV_H

#include "lwip/opt.h"

#if LWIP_RAW /* don't build if not configured for use in lwipopts.h */

#include "lwip/raw.h"

#ifdef __cplusplus
extern "C" {
#endif

/** return codes for raw_input */
typedef enum raw_input_state
{
  RAW_INPUT_NONE = 0, /* pbuf did not match any pcbs */
  RAW_INPUT_EATEN,    /* pbuf handed off and delivered to pcb */
  RAW_INPUT_DELIVERED /* pbuf only delivered to pcb (pbuf can still be referenced) */
} raw_input_state_t;

/* The following functions are the lower layer interface to RAW. */
raw_input_state_t raw_input(struct pbuf *p, struct netif *inp);

void raw_netif_ip_addr_changed(const ip_addr_t* old_addr, const ip_addr_t* new_addr);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_RAW */

#endif /* LWIP_HDR_RAW_PRIV_H */
