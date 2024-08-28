

/**
 * @defgroup bridgeif_fdb FDB example code
 * @ingroup bridgeif
 * This file implements an example for an FDB (Forwarding DataBase)
 */

#include "netif/bridgeif.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/timeouts.h"
#include <string.h>

#define BRIDGEIF_AGE_TIMER_MS 1000

#define BR_FDB_TIMEOUT_SEC  (60*5) /* 5 minutes FDB timeout */

typedef struct bridgeif_dfdb_entry_s {
  u8_t used;
  u8_t port;
  u32_t ts;
  struct eth_addr addr;
} bridgeif_dfdb_entry_t;

typedef struct bridgeif_dfdb_s {
  u16_t max_fdb_entries;
  bridgeif_dfdb_entry_t *fdb;
} bridgeif_dfdb_t;

/**
 * @ingroup bridgeif_fdb
 * A real simple and slow implementation of an auto-learning forwarding database that
 * remembers known src mac addresses to know which port to send frames destined for that
 * mac address.
 *
 * ATTENTION: This is meant as an example only, in real-world use, you should 
 * provide a better implementation :-)
 */
void
bridgeif_fdb_update_src(void *fdb_ptr, struct eth_addr *src_addr, u8_t port_idx)
{
  int i;
  bridgeif_dfdb_t *fdb = (bridgeif_dfdb_t *)fdb_ptr;
  BRIDGEIF_DECL_PROTECT(lev);
  BRIDGEIF_READ_PROTECT(lev);
  for (i = 0; i < fdb->max_fdb_entries; i++) {
    bridgeif_dfdb_entry_t *e = &fdb->fdb[i];
    if (e->used && e->ts) {
      if (!memcmp(&e->addr, src_addr, sizeof(struct eth_addr))) {
        LWIP_DEBUGF(BRIDGEIF_FDB_DEBUG, ("br: update src %02x:%02x:%02x:%02x:%02x:%02x (from %d) @ idx %d\n",
                                         src_addr->addr[0], src_addr->addr[1], src_addr->addr[2], src_addr->addr[3], src_addr->addr[4], src_addr->addr[5],
                                         port_idx, i));
        BRIDGEIF_WRITE_PROTECT(lev);
        e->ts = BR_FDB_TIMEOUT_SEC;
        e->port = port_idx;
        BRIDGEIF_WRITE_UNPROTECT(lev);
        BRIDGEIF_READ_UNPROTECT(lev);
        return;
      }
    }
  }
  /* not found, allocate new entry from free */
  for (i = 0; i < fdb->max_fdb_entries; i++) {
    bridgeif_dfdb_entry_t *e = &fdb->fdb[i];
    if (!e->used || !e->ts) {
      BRIDGEIF_WRITE_PROTECT(lev);
      /* check again when protected */
      if (!e->used || !e->ts) {
        LWIP_DEBUGF(BRIDGEIF_FDB_DEBUG, ("br: create src %02x:%02x:%02x:%02x:%02x:%02x (from %d) @ idx %d\n",
                                         src_addr->addr[0], src_addr->addr[1], src_addr->addr[2], src_addr->addr[3], src_addr->addr[4], src_addr->addr[5],
                                         port_idx, i));
        memcpy(&e->addr, src_addr, sizeof(struct eth_addr));
        e->ts = BR_FDB_TIMEOUT_SEC;
        e->port = port_idx;
        e->used = 1;
        BRIDGEIF_WRITE_UNPROTECT(lev);
        BRIDGEIF_READ_UNPROTECT(lev);
        return;
      }
      BRIDGEIF_WRITE_UNPROTECT(lev);
    }
  }
  BRIDGEIF_READ_UNPROTECT(lev);
  /* not found, no free entry -> flood */
}

/** 
 * @ingroup bridgeif_fdb
 * Walk our list of auto-learnt fdb entries and return a port to forward or BR_FLOOD if unknown 
 */
bridgeif_portmask_t
bridgeif_fdb_get_dst_ports(void *fdb_ptr, struct eth_addr *dst_addr)
{
  int i;
  bridgeif_dfdb_t *fdb = (bridgeif_dfdb_t *)fdb_ptr;
  BRIDGEIF_DECL_PROTECT(lev);
  BRIDGEIF_READ_PROTECT(lev);
  for (i = 0; i < fdb->max_fdb_entries; i++) {
    bridgeif_dfdb_entry_t *e = &fdb->fdb[i];
    if (e->used && e->ts) {
      if (!memcmp(&e->addr, dst_addr, sizeof(struct eth_addr))) {
        bridgeif_portmask_t ret = (bridgeif_portmask_t)(1 << e->port);
        BRIDGEIF_READ_UNPROTECT(lev);
        return ret;
      }
    }
  }
  BRIDGEIF_READ_UNPROTECT(lev);
  return BR_FLOOD;
}

/**
 * @ingroup bridgeif_fdb
 * Aging implementation of our simple fdb
 */
static void
bridgeif_fdb_age_one_second(void *fdb_ptr)
{
  int i;
  bridgeif_dfdb_t *fdb;
  BRIDGEIF_DECL_PROTECT(lev);

  fdb = (bridgeif_dfdb_t *)fdb_ptr;
  BRIDGEIF_READ_PROTECT(lev);

  for (i = 0; i < fdb->max_fdb_entries; i++) {
    bridgeif_dfdb_entry_t *e = &fdb->fdb[i];
    if (e->used && e->ts) {
      BRIDGEIF_WRITE_PROTECT(lev);
      /* check again when protected */
      if (e->used && e->ts) {
        if (--e->ts == 0) {
          e->used = 0;
        }
      }
      BRIDGEIF_WRITE_UNPROTECT(lev);
    }
  }
  BRIDGEIF_READ_UNPROTECT(lev);
}

/** Timer callback for fdb aging, called once per second */
static void
bridgeif_age_tmr(void *arg)
{
  bridgeif_dfdb_t *fdb = (bridgeif_dfdb_t *)arg;

  LWIP_ASSERT("invalid arg", arg != NULL);

  bridgeif_fdb_age_one_second(fdb);
  sys_timeout(BRIDGEIF_AGE_TIMER_MS, bridgeif_age_tmr, arg);
}

/**
 * @ingroup bridgeif_fdb
 * Init our simple fdb list
 */
void *
bridgeif_fdb_init(u16_t max_fdb_entries)
{
  bridgeif_dfdb_t *fdb;
  size_t alloc_len_sizet = sizeof(bridgeif_dfdb_t) + (max_fdb_entries * sizeof(bridgeif_dfdb_entry_t));
  mem_size_t alloc_len = (mem_size_t)alloc_len_sizet;
  LWIP_ASSERT("alloc_len == alloc_len_sizet", alloc_len == alloc_len_sizet);
  LWIP_DEBUGF(BRIDGEIF_DEBUG, ("bridgeif_fdb_init: allocating %d bytes for private FDB data\n", (int)alloc_len));
  fdb = (bridgeif_dfdb_t *)mem_calloc(1, alloc_len);
  if (fdb == NULL) {
    return NULL;
  }
  fdb->max_fdb_entries = max_fdb_entries;
  fdb->fdb = (bridgeif_dfdb_entry_t *)(fdb + 1);

  sys_timeout(BRIDGEIF_AGE_TIMER_MS, bridgeif_age_tmr, fdb);

  return fdb;
}
