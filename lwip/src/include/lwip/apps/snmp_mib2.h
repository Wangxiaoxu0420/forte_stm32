
#ifndef LWIP_HDR_APPS_SNMP_MIB2_H
#define LWIP_HDR_APPS_SNMP_MIB2_H

#include "lwip/apps/snmp_opts.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_SNMP /* don't build if not configured for use in lwipopts.h */
#if SNMP_LWIP_MIB2

#include "lwip/apps/snmp_core.h"

extern const struct snmp_mib mib2;

#if SNMP_USE_NETCONN
#include "lwip/apps/snmp_threadsync.h"
void snmp_mib2_lwip_synchronizer(snmp_threadsync_called_fn fn, void* arg);
extern struct snmp_threadsync_instance snmp_mib2_lwip_locks;
#endif

#ifndef SNMP_SYSSERVICES
#define SNMP_SYSSERVICES ((1 << 6) | (1 << 3) | ((IP_FORWARD) << 2))
#endif

void snmp_mib2_set_sysdescr(const u8_t* str, const u16_t* len); /* read-only be defintion */
void snmp_mib2_set_syscontact(u8_t *ocstr, u16_t *ocstrlen, u16_t bufsize);
void snmp_mib2_set_syscontact_readonly(const u8_t *ocstr, const u16_t *ocstrlen);
void snmp_mib2_set_sysname(u8_t *ocstr, u16_t *ocstrlen, u16_t bufsize);
void snmp_mib2_set_sysname_readonly(const u8_t *ocstr, const u16_t *ocstrlen);
void snmp_mib2_set_syslocation(u8_t *ocstr, u16_t *ocstrlen, u16_t bufsize);
void snmp_mib2_set_syslocation_readonly(const u8_t *ocstr, const u16_t *ocstrlen);

#endif /* SNMP_LWIP_MIB2 */
#endif /* LWIP_SNMP */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_SNMP_MIB2_H */
