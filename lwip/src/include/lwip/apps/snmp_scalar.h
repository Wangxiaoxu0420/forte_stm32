

#ifndef LWIP_HDR_APPS_SNMP_SCALAR_H
#define LWIP_HDR_APPS_SNMP_SCALAR_H

#include "lwip/apps/snmp_opts.h"
#include "lwip/apps/snmp_core.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_SNMP /* don't build if not configured for use in lwipopts.h */

/** basic scalar node */
struct snmp_scalar_node
{
  /** inherited "base class" members */
  struct snmp_leaf_node node;
  u8_t asn1_type;
  snmp_access_t access;
  node_instance_get_value_method get_value;
  node_instance_set_test_method set_test;
  node_instance_set_value_method set_value;
};


snmp_err_t snmp_scalar_get_instance(const u32_t *root_oid, u8_t root_oid_len, struct snmp_node_instance* instance);
snmp_err_t snmp_scalar_get_next_instance(const u32_t *root_oid, u8_t root_oid_len, struct snmp_node_instance* instance);

#define SNMP_SCALAR_CREATE_NODE(oid, access, asn1_type, get_value_method, set_test_method, set_value_method) \
  {{{ SNMP_NODE_SCALAR, (oid) }, \
    snmp_scalar_get_instance, \
    snmp_scalar_get_next_instance }, \
    (asn1_type), (access), (get_value_method), (set_test_method), (set_value_method) }

#define SNMP_SCALAR_CREATE_NODE_READONLY(oid, asn1_type, get_value_method) SNMP_SCALAR_CREATE_NODE(oid, SNMP_NODE_INSTANCE_READ_ONLY, asn1_type, get_value_method, NULL, NULL)

/** scalar array node - a tree node which contains scalars only as children */
struct snmp_scalar_array_node_def
{
  u32_t         oid;
  u8_t          asn1_type;
  snmp_access_t access;
};

typedef s16_t (*snmp_scalar_array_get_value_method)(const struct snmp_scalar_array_node_def*, void*);
typedef snmp_err_t (*snmp_scalar_array_set_test_method)(const struct snmp_scalar_array_node_def*, u16_t, void*);
typedef snmp_err_t (*snmp_scalar_array_set_value_method)(const struct snmp_scalar_array_node_def*, u16_t, void*);

/** basic scalar array node */
struct snmp_scalar_array_node
{
  /** inherited "base class" members */
  struct snmp_leaf_node node;
  u16_t array_node_count;
  const struct snmp_scalar_array_node_def* array_nodes;
  snmp_scalar_array_get_value_method get_value;
  snmp_scalar_array_set_test_method set_test;
  snmp_scalar_array_set_value_method set_value;
};

snmp_err_t snmp_scalar_array_get_instance(const u32_t *root_oid, u8_t root_oid_len, struct snmp_node_instance* instance);
snmp_err_t snmp_scalar_array_get_next_instance(const u32_t *root_oid, u8_t root_oid_len, struct snmp_node_instance* instance);

#define SNMP_SCALAR_CREATE_ARRAY_NODE(oid, array_nodes, get_value_method, set_test_method, set_value_method) \
  {{{ SNMP_NODE_SCALAR_ARRAY, (oid) }, \
    snmp_scalar_array_get_instance, \
    snmp_scalar_array_get_next_instance }, \
    (u16_t)LWIP_ARRAYSIZE(array_nodes), (array_nodes), (get_value_method), (set_test_method), (set_value_method) }

#endif /* LWIP_SNMP */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_SNMP_SCALAR_H */
