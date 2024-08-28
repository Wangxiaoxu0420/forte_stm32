

#ifndef LWIP_HDR_APPS_SNMP_V3_H
#define LWIP_HDR_APPS_SNMP_V3_H

#include "lwip/apps/snmp_opts.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_SNMP && LWIP_SNMP_V3

typedef enum
{
  SNMP_V3_AUTH_ALGO_INVAL = 0,
  SNMP_V3_AUTH_ALGO_MD5   = 1,
  SNMP_V3_AUTH_ALGO_SHA   = 2
} snmpv3_auth_algo_t;

typedef enum
{
  SNMP_V3_PRIV_ALGO_INVAL = 0,
  SNMP_V3_PRIV_ALGO_DES   = 1,
  SNMP_V3_PRIV_ALGO_AES   = 2
} snmpv3_priv_algo_t;

typedef enum
{
  SNMP_V3_USER_STORAGETYPE_OTHER       = 1,
  SNMP_V3_USER_STORAGETYPE_VOLATILE    = 2,
  SNMP_V3_USER_STORAGETYPE_NONVOLATILE = 3,
  SNMP_V3_USER_STORAGETYPE_PERMANENT   = 4,
  SNMP_V3_USER_STORAGETYPE_READONLY    = 5
} snmpv3_user_storagetype_t;

/*
 * The following callback functions must be implemented by the application.
 * There is a dummy implementation in snmpv3_dummy.c.
 */

void snmpv3_get_engine_id(const char **id, u8_t *len);
err_t snmpv3_set_engine_id(const char* id, u8_t len);

u32_t snmpv3_get_engine_boots(void);
void snmpv3_set_engine_boots(u32_t boots);

u32_t snmpv3_get_engine_time(void);
void snmpv3_reset_engine_time(void);

err_t snmpv3_get_user(const char* username, snmpv3_auth_algo_t *auth_algo, u8_t *auth_key, snmpv3_priv_algo_t *priv_algo, u8_t *priv_key);
u8_t snmpv3_get_amount_of_users(void);
err_t snmpv3_get_user_storagetype(const char *username, snmpv3_user_storagetype_t *storagetype);
err_t snmpv3_get_username(char *username, u8_t index);

/* The following functions are provided by the SNMPv3 agent */

void snmpv3_engine_id_changed(void);
s32_t snmpv3_get_engine_time_internal(void);

void snmpv3_password_to_key_md5(
    const u8_t *password,     /* IN */
    size_t      passwordlen,  /* IN */
    const u8_t *engineID,     /* IN  - pointer to snmpEngineID  */
    u8_t        engineLength, /* IN  - length of snmpEngineID */
    u8_t       *key);         /* OUT - pointer to caller 16-octet buffer */

void snmpv3_password_to_key_sha(
    const u8_t *password,     /* IN */
    size_t      passwordlen,  /* IN */
    const u8_t *engineID,     /* IN  - pointer to snmpEngineID  */
    u8_t        engineLength, /* IN  - length of snmpEngineID */
    u8_t       *key);         /* OUT - pointer to caller 20-octet buffer */

#endif

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_SNMP_V3_H */
