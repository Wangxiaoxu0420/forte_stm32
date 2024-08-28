

#include "snmpv3_priv.h"
#include "lwip/apps/snmpv3.h"
#include "lwip/sys.h"
#include <string.h>

#if LWIP_SNMP && LWIP_SNMP_V3

#ifdef LWIP_SNMPV3_INCLUDE_ENGINE
#include LWIP_SNMPV3_INCLUDE_ENGINE
#endif

#define SNMP_MAX_TIME_BOOT 2147483647UL

/** Call this if engine has been changed. Has to reset boots, see below */
void
snmpv3_engine_id_changed(void)
{
  snmpv3_set_engine_boots(0);
}

/** According to RFC3414 2.2.2.
 *
 * The number of times that the SNMP engine has
 * (re-)initialized itself since snmpEngineID
 * was last configured.
 */
s32_t
snmpv3_get_engine_boots_internal(void)
{
  if (snmpv3_get_engine_boots() == 0 ||
      snmpv3_get_engine_boots() < SNMP_MAX_TIME_BOOT) {
    return snmpv3_get_engine_boots();
  }

  snmpv3_set_engine_boots(SNMP_MAX_TIME_BOOT);
  return snmpv3_get_engine_boots();
}

/** RFC3414 2.2.2.
 *
 * Once the timer reaches 2147483647 it gets reset to zero and the
 * engine boot ups get incremented.
 */
s32_t
snmpv3_get_engine_time_internal(void)
{
  if (snmpv3_get_engine_time() >= SNMP_MAX_TIME_BOOT) {
    snmpv3_reset_engine_time();

    if (snmpv3_get_engine_boots() < SNMP_MAX_TIME_BOOT - 1) {
      snmpv3_set_engine_boots(snmpv3_get_engine_boots() + 1);
    } else {
      snmpv3_set_engine_boots(SNMP_MAX_TIME_BOOT);
    }
  }

  return snmpv3_get_engine_time();
}

#if LWIP_SNMP_V3_CRYPTO

/* This function ignores the byte order suggestion in RFC3414
 * since it simply doesn't influence the effectiveness of an IV.
 *
 * Implementing RFC3826 priv param algorithm if LWIP_RAND is available.
 *
 * @todo: This is a potential thread safety issue.
 */
err_t
snmpv3_build_priv_param(u8_t *priv_param)
{
#ifdef LWIP_RAND /* Based on RFC3826 */
  static u8_t init;
  static u32_t priv1, priv2;

  /* Lazy initialisation */
  if (init == 0) {
    init = 1;
    priv1 = LWIP_RAND();
    priv2 = LWIP_RAND();
  }

  SMEMCPY(&priv_param[0], &priv1, sizeof(priv1));
  SMEMCPY(&priv_param[4], &priv2, sizeof(priv2));

  /* Emulate 64bit increment */
  priv1++;
  if (!priv1) { /* Overflow */
    priv2++;
  }
#else /* Based on RFC3414 */
  static u32_t ctr;
  u32_t boots = snmpv3_get_engine_boots_internal();
  SMEMCPY(&priv_param[0], &boots, 4);
  SMEMCPY(&priv_param[4], &ctr, 4);
  ctr++;
#endif
  return ERR_OK;
}
#endif /* LWIP_SNMP_V3_CRYPTO */

#endif
