

#ifndef LWIP_HDR_APPS_SNMP_V3_PRIV_H
#define LWIP_HDR_APPS_SNMP_V3_PRIV_H

#include "lwip/apps/snmp_opts.h"

#if LWIP_SNMP && LWIP_SNMP_V3

#include "lwip/apps/snmpv3.h"
#include "snmp_pbuf_stream.h"

/* According to RFC 3411 */
#define SNMP_V3_MAX_ENGINE_ID_LENGTH  32
#define SNMP_V3_MAX_USER_LENGTH       32

#define SNMP_V3_MAX_AUTH_PARAM_LENGTH  12
#define SNMP_V3_MAX_PRIV_PARAM_LENGTH  8

#define SNMP_V3_MD5_LEN        16
#define SNMP_V3_SHA_LEN        20

typedef enum {
  SNMP_V3_PRIV_MODE_DECRYPT = 0,
  SNMP_V3_PRIV_MODE_ENCRYPT = 1
} snmpv3_priv_mode_t;

s32_t snmpv3_get_engine_boots_internal(void);
err_t snmpv3_auth(struct snmp_pbuf_stream *stream, u16_t length, const u8_t *key, snmpv3_auth_algo_t algo, u8_t *hmac_out);
err_t snmpv3_crypt(struct snmp_pbuf_stream *stream, u16_t length, const u8_t *key,
                   const u8_t *priv_param, const u32_t engine_boots, const u32_t engine_time, snmpv3_priv_algo_t algo, snmpv3_priv_mode_t mode);
err_t snmpv3_build_priv_param(u8_t *priv_param);
void snmpv3_enginetime_timer(void *arg);

#endif

#endif /* LWIP_HDR_APPS_SNMP_V3_PRIV_H */
