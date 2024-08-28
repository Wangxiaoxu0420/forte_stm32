
#ifndef LWIP_HDR_APPS_SNMP_H
#define LWIP_HDR_APPS_SNMP_H

#include "lwip/apps/snmp_opts.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_SNMP /* don't build if not configured for use in lwipopts.h */

#include "lwip/err.h"
#include "lwip/apps/snmp_core.h"

/** SNMP variable binding descriptor (publically needed for traps) */
struct snmp_varbind
{
  /** pointer to next varbind, NULL for last in list */
  struct snmp_varbind *next;
  /** pointer to previous varbind, NULL for first in list */
  struct snmp_varbind *prev;

  /** object identifier */
  struct snmp_obj_id oid;

  /** value ASN1 type */
  u8_t type;
  /** object value length */
  u16_t value_len;
  /** object value */
  void *value;
};

/**
 * @ingroup snmp_core
 * Agent setup, start listening to port 161.
 */
void snmp_init(void);
void snmp_set_mibs(const struct snmp_mib **mibs, u8_t num_mibs);

void snmp_set_device_enterprise_oid(const struct snmp_obj_id* device_enterprise_oid);
const struct snmp_obj_id* snmp_get_device_enterprise_oid(void);

void snmp_trap_dst_enable(u8_t dst_idx, u8_t enable);
void snmp_trap_dst_ip_set(u8_t dst_idx, const ip_addr_t *dst);

/** Generic trap: cold start */
#define SNMP_GENTRAP_COLDSTART 0
/** Generic trap: warm start */
#define SNMP_GENTRAP_WARMSTART 1
/** Generic trap: link down */
#define SNMP_GENTRAP_LINKDOWN 2
/** Generic trap: link up */
#define SNMP_GENTRAP_LINKUP 3
/** Generic trap: authentication failure */
#define SNMP_GENTRAP_AUTH_FAILURE 4
/** Generic trap: EGP neighbor lost */
#define SNMP_GENTRAP_EGP_NEIGHBOR_LOSS 5
/** Generic trap: enterprise specific */
#define SNMP_GENTRAP_ENTERPRISE_SPECIFIC 6

err_t snmp_send_trap_generic(s32_t generic_trap);
err_t snmp_send_trap_specific(s32_t specific_trap, struct snmp_varbind *varbinds);
err_t snmp_send_trap(const struct snmp_obj_id* oid, s32_t generic_trap, s32_t specific_trap, struct snmp_varbind *varbinds);

#define SNMP_AUTH_TRAPS_DISABLED 0
#define SNMP_AUTH_TRAPS_ENABLED  1
void snmp_set_auth_traps_enabled(u8_t enable);
u8_t snmp_get_auth_traps_enabled(void);

u8_t snmp_v1_enabled(void);
u8_t snmp_v2c_enabled(void);
u8_t snmp_v3_enabled(void);
void snmp_v1_enable(u8_t enable);
void snmp_v2c_enable(u8_t enable);
void snmp_v3_enable(u8_t enable);

const char * snmp_get_community(void);
const char * snmp_get_community_write(void);
const char * snmp_get_community_trap(void);
void snmp_set_community(const char * const community);
void snmp_set_community_write(const char * const community);
void snmp_set_community_trap(const char * const community);

void snmp_coldstart_trap(void);
void snmp_authfail_trap(void);

typedef void (*snmp_write_callback_fct)(const u32_t* oid, u8_t oid_len, void* callback_arg);
void snmp_set_write_callback(snmp_write_callback_fct write_callback, void* callback_arg);

#endif /* LWIP_SNMP */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_SNMP_H */
