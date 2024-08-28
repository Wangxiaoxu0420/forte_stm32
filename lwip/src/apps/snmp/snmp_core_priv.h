

#ifndef LWIP_HDR_APPS_SNMP_CORE_PRIV_H
#define LWIP_HDR_APPS_SNMP_CORE_PRIV_H

#include "lwip/apps/snmp_opts.h"

#if LWIP_SNMP /* don't build if not configured for use in lwipopts.h */

#include "lwip/apps/snmp_core.h"
#include "snmp_asn1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* (outdated) SNMPv1 error codes
 * shall not be used by MIBS anymore, nevertheless required from core for properly answering a v1 request
 */
#define SNMP_ERR_NOSUCHNAME 2
#define SNMP_ERR_BADVALUE   3
#define SNMP_ERR_READONLY   4
/* error codes which are internal and shall not be used by MIBS
 * shall not be used by MIBS anymore, nevertheless required from core for properly answering a v1 request
 */
#define SNMP_ERR_TOOBIG               1
#define SNMP_ERR_AUTHORIZATIONERROR   16

#define SNMP_ERR_UNKNOWN_ENGINEID     30
#define SNMP_ERR_UNKNOWN_SECURITYNAME 31
#define SNMP_ERR_UNSUPPORTED_SECLEVEL 32
#define SNMP_ERR_NOTINTIMEWINDOW      33
#define SNMP_ERR_DECRYIPTION_ERROR    34

#define SNMP_ERR_NOSUCHOBJECT         SNMP_VARBIND_EXCEPTION_OFFSET + SNMP_ASN1_CONTEXT_VARBIND_NO_SUCH_OBJECT
#define SNMP_ERR_ENDOFMIBVIEW         SNMP_VARBIND_EXCEPTION_OFFSET + SNMP_ASN1_CONTEXT_VARBIND_END_OF_MIB_VIEW


const struct snmp_node *snmp_mib_tree_resolve_exact(const struct snmp_mib *mib, const u32_t *oid, u8_t oid_len, u8_t *oid_instance_len);
const struct snmp_node *snmp_mib_tree_resolve_next(const struct snmp_mib *mib, const u32_t *oid, u8_t oid_len, struct snmp_obj_id *oidret);

typedef u8_t (*snmp_validate_node_instance_method)(struct snmp_node_instance *, void *);

u8_t snmp_get_node_instance_from_oid(const u32_t *oid, u8_t oid_len, struct snmp_node_instance *node_instance);
u8_t snmp_get_next_node_instance_from_oid(const u32_t *oid, u8_t oid_len, snmp_validate_node_instance_method validate_node_instance_method, void *validate_node_instance_arg, struct snmp_obj_id *node_oid, struct snmp_node_instance *node_instance);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_SNMP */

#endif /* LWIP_HDR_APPS_SNMP_CORE_PRIV_H */
