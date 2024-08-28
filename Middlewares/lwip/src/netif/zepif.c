

#include "netif/zepif.h"

#if LWIP_IPV6 && LWIP_UDP

#include "netif/lowpan6.h"
#include "lwip/udp.h"
#include "lwip/timeouts.h"
#include <string.h>

/** Define this to 1 to loop back TX packets for testing */
#ifndef ZEPIF_LOOPBACK
#define ZEPIF_LOOPBACK    0
#endif

#define ZEP_MAX_DATA_LEN  127

#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct zep_hdr {
  PACK_STRUCT_FLD_8(u8_t prot_id[2]);
  PACK_STRUCT_FLD_8(u8_t prot_version);
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t channel_id);
  PACK_STRUCT_FIELD(u16_t device_id);
  PACK_STRUCT_FLD_8(u8_t crc_mode);
  PACK_STRUCT_FLD_8(u8_t unknown_1);
  PACK_STRUCT_FIELD(u32_t timestamp[2]);
  PACK_STRUCT_FIELD(u32_t seq_num);
  PACK_STRUCT_FLD_8(u8_t unknown_2[10]);
  PACK_STRUCT_FLD_8(u8_t len);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

struct zepif_state {
  struct zepif_init init;
  struct udp_pcb *pcb;
  u32_t seqno;
};

static u8_t zep_lowpan_timer_running;

/* Helper function that calls the 6LoWPAN timer and reschedules itself */
static void
zep_lowpan_timer(void *arg)
{
  lowpan6_tmr();
  if (zep_lowpan_timer_running) {
    sys_timeout(LOWPAN6_TMR_INTERVAL, zep_lowpan_timer, arg);
  }
}

/* Pass received pbufs into 6LowPAN netif */
static void
zepif_udp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,
               const ip_addr_t *addr, u16_t port)
{
  err_t err;
  struct netif *netif_lowpan6 = (struct netif *)arg;
  struct zep_hdr *zep;

  LWIP_ASSERT("arg != NULL", arg != NULL);
  LWIP_ASSERT("pcb != NULL", pcb != NULL);
  LWIP_UNUSED_ARG(pcb); /* for LWIP_NOASSERT */
  LWIP_UNUSED_ARG(addr);
  LWIP_UNUSED_ARG(port);
  if (p == NULL) {
    return;
  }

  /* Parse and hide the ZEP header */
  if (p->len < sizeof(struct zep_hdr)) {
    /* need the zep_hdr in one piece */
    goto err_return;
  }
  zep = (struct zep_hdr *)p->payload;
  if (zep->prot_id[0] != 'E') {
    goto err_return;
  }
  if (zep->prot_id[1] != 'X') {
    goto err_return;
  }
  if (zep->prot_version != 2) {
    /* we only support this version for now */
    goto err_return;
  }
  if (zep->type != 1) {
    goto err_return;
  }
  if (zep->crc_mode != 1) {
    goto err_return;
  }
  if (zep->len != p->tot_len - sizeof(struct zep_hdr)) {
    goto err_return;
  }
  /* everything seems to be OK, hide the ZEP header */
  if (pbuf_remove_header(p, sizeof(struct zep_hdr))) {
    goto err_return;
  }
  /* TODO Check CRC? */
  /* remove CRC trailer */
  pbuf_realloc(p, p->tot_len - 2);

  /* Call into 6LoWPAN code. */
  err = netif_lowpan6->input(p, netif_lowpan6);
  if (err == ERR_OK) {
    return;
  }
err_return:
  pbuf_free(p);
}

/* Send 6LoWPAN TX packets as UDP broadcast */
static err_t
zepif_linkoutput(struct netif *netif, struct pbuf *p)
{
  err_t err;
  struct pbuf *q;
  struct zep_hdr *zep;
  struct zepif_state *state;

  LWIP_ASSERT("invalid netif", netif != NULL);
  LWIP_ASSERT("invalid pbuf", p != NULL);

  if (p->tot_len > ZEP_MAX_DATA_LEN) {
    return ERR_VAL;
  }
  LWIP_ASSERT("TODO: support chained pbufs", p->next == NULL);

  state = (struct zepif_state *)netif->state;
  LWIP_ASSERT("state->pcb != NULL", state->pcb != NULL);

  q = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct zep_hdr) + p->tot_len, PBUF_RAM);
  if (q == NULL) {
    return ERR_MEM;
  }
  zep = (struct zep_hdr *)q->payload;
  memset(zep, 0, sizeof(struct zep_hdr));
  zep->prot_id[0] = 'E';
  zep->prot_id[1] = 'X';
  zep->prot_version = 2;
  zep->type = 1; /* Data */
  zep->channel_id = 0; /* whatever */
  zep->device_id = lwip_htons(1); /* whatever */
  zep->crc_mode = 1;
  zep->unknown_1 = 0xff;
  zep->seq_num = lwip_htonl(state->seqno);
  state->seqno++;
  zep->len = (u8_t)p->tot_len;

  err = pbuf_copy_partial_pbuf(q, p, p->tot_len, sizeof(struct zep_hdr));
  if (err == ERR_OK) {
#if ZEPIF_LOOPBACK
    zepif_udp_recv(netif, state->pcb, pbuf_clone(PBUF_RAW, PBUF_RAM, q), NULL, 0);
#endif
    err = udp_sendto(state->pcb, q, state->init.zep_dst_ip_addr, state->init.zep_dst_udp_port);
  }
  pbuf_free(q);

  return err;
}

/**
 * @ingroup zepif
 * Set up a raw 6LowPAN netif and surround it with input- and output
 * functions for ZEP
 */
err_t
zepif_init(struct netif *netif)
{
  err_t err;
  struct zepif_init *init_state = (struct zepif_init *)netif->state;
  struct zepif_state *state = (struct zepif_state *)mem_malloc(sizeof(struct zepif_state));

  LWIP_ASSERT("zepif needs an input callback", netif->input != NULL);

  if (state == NULL) {
    return ERR_MEM;
  }
  memset(state, 0, sizeof(struct zepif_state));
  if (init_state != NULL) {
    memcpy(&state->init, init_state, sizeof(struct zepif_init));
  }
  if (state->init.zep_src_udp_port == 0) {
    state->init.zep_src_udp_port = ZEPIF_DEFAULT_UDP_PORT;
  }
  if (state->init.zep_dst_udp_port == 0) {
    state->init.zep_dst_udp_port = ZEPIF_DEFAULT_UDP_PORT;
  }
#if LWIP_IPV4
  if (state->init.zep_dst_ip_addr == NULL) {
    /* With IPv4 enabled, default to broadcasting packets if no address is set */
    state->init.zep_dst_ip_addr = IP_ADDR_BROADCAST;
  }
#endif /* LWIP_IPV4 */

  netif->state = NULL;

  state->pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  if (state->pcb == NULL) {
    err = ERR_MEM;
    goto err_ret;
  }
  err = udp_bind(state->pcb, state->init.zep_src_ip_addr, state->init.zep_src_udp_port);
  if (err != ERR_OK) {
    goto err_ret;
  }
  if (state->init.zep_netif != NULL) {
    udp_bind_netif(state->pcb, state->init.zep_netif);
  }
  LWIP_ASSERT("udp_bind(lowpan6_broadcast_pcb) failed", err == ERR_OK);
  ip_set_option(state->pcb, SOF_BROADCAST);
  udp_recv(state->pcb, zepif_udp_recv, netif);

  err = lowpan6_if_init(netif);
  LWIP_ASSERT("lowpan6_if_init set a state", netif->state == NULL);
  if (err == ERR_OK) {
    netif->state = state;
    netif->hwaddr_len = 6;
    if (init_state != NULL) {
      memcpy(netif->hwaddr, init_state->addr, 6);
    } else {
      u8_t i;
      for (i = 0; i < 6; i++) {
        netif->hwaddr[i] = i;
      }
      netif->hwaddr[0] &= 0xfc;
    }
    netif->linkoutput = zepif_linkoutput;

    if (!zep_lowpan_timer_running) {
      sys_timeout(LOWPAN6_TMR_INTERVAL, zep_lowpan_timer, NULL);
      zep_lowpan_timer_running = 1;
    }

    return ERR_OK;
  }

err_ret:
  if (state->pcb != NULL) {
    udp_remove(state->pcb);
  }
  mem_free(state);
  return err;
}

#endif /* LWIP_IPV6 && LWIP_UDP */
