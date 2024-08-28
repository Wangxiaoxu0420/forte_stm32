

#include "lwip/snmp.h"
#include "lwip/apps/snmp.h"
#include "lwip/apps/snmp_core.h"
#include "lwip/apps/snmp_mib2.h"
#include "lwip/apps/snmp_table.h"
#include "lwip/apps/snmp_scalar.h"
#include "lwip/icmp.h"
#include "lwip/stats.h"

#if LWIP_SNMP && SNMP_LWIP_MIB2 && LWIP_ICMP

#if SNMP_USE_NETCONN
#define SYNC_NODE_NAME(node_name) node_name ## _synced
#define CREATE_LWIP_SYNC_NODE(oid, node_name) \
   static const struct snmp_threadsync_node node_name ## _synced = SNMP_CREATE_THREAD_SYNC_NODE(oid, &node_name.node, &snmp_mib2_lwip_locks);
#else
#define SYNC_NODE_NAME(node_name) node_name
#define CREATE_LWIP_SYNC_NODE(oid, node_name)
#endif

/* --- icmp .1.3.6.1.2.1.5 ----------------------------------------------------- */

static s16_t
icmp_get_value(const struct snmp_scalar_array_node_def *node, void *value)
{
  u32_t *uint_ptr = (u32_t *)value;

  switch (node->oid) {
    case 1: /* icmpInMsgs */
      *uint_ptr = STATS_GET(mib2.icmpinmsgs);
      return sizeof(*uint_ptr);
    case 2: /* icmpInErrors */
      *uint_ptr = STATS_GET(mib2.icmpinerrors);
      return sizeof(*uint_ptr);
    case 3: /* icmpInDestUnreachs */
      *uint_ptr = STATS_GET(mib2.icmpindestunreachs);
      return sizeof(*uint_ptr);
    case 4: /* icmpInTimeExcds */
      *uint_ptr = STATS_GET(mib2.icmpintimeexcds);
      return sizeof(*uint_ptr);
    case 5: /* icmpInParmProbs */
      *uint_ptr = STATS_GET(mib2.icmpinparmprobs);
      return sizeof(*uint_ptr);
    case 6: /* icmpInSrcQuenchs */
      *uint_ptr = STATS_GET(mib2.icmpinsrcquenchs);
      return sizeof(*uint_ptr);
    case 7: /* icmpInRedirects */
      *uint_ptr = STATS_GET(mib2.icmpinredirects);
      return sizeof(*uint_ptr);
    case 8: /* icmpInEchos */
      *uint_ptr = STATS_GET(mib2.icmpinechos);
      return sizeof(*uint_ptr);
    case 9: /* icmpInEchoReps */
      *uint_ptr = STATS_GET(mib2.icmpinechoreps);
      return sizeof(*uint_ptr);
    case 10: /* icmpInTimestamps */
      *uint_ptr = STATS_GET(mib2.icmpintimestamps);
      return sizeof(*uint_ptr);
    case 11: /* icmpInTimestampReps */
      *uint_ptr = STATS_GET(mib2.icmpintimestampreps);
      return sizeof(*uint_ptr);
    case 12: /* icmpInAddrMasks */
      *uint_ptr = STATS_GET(mib2.icmpinaddrmasks);
      return sizeof(*uint_ptr);
    case 13: /* icmpInAddrMaskReps */
      *uint_ptr = STATS_GET(mib2.icmpinaddrmaskreps);
      return sizeof(*uint_ptr);
    case 14: /* icmpOutMsgs */
      *uint_ptr = STATS_GET(mib2.icmpoutmsgs);
      return sizeof(*uint_ptr);
    case 15: /* icmpOutErrors */
      *uint_ptr = STATS_GET(mib2.icmpouterrors);
      return sizeof(*uint_ptr);
    case 16: /* icmpOutDestUnreachs */
      *uint_ptr = STATS_GET(mib2.icmpoutdestunreachs);
      return sizeof(*uint_ptr);
    case 17: /* icmpOutTimeExcds */
      *uint_ptr = STATS_GET(mib2.icmpouttimeexcds);
      return sizeof(*uint_ptr);
    case 18: /* icmpOutParmProbs: not supported -> always 0 */
      *uint_ptr = 0;
      return sizeof(*uint_ptr);
    case 19: /* icmpOutSrcQuenchs: not supported -> always 0 */
      *uint_ptr = 0;
      return sizeof(*uint_ptr);
    case 20: /* icmpOutRedirects: not supported -> always 0 */
      *uint_ptr = 0;
      return sizeof(*uint_ptr);
    case 21: /* icmpOutEchos */
      *uint_ptr = STATS_GET(mib2.icmpoutechos);
      return sizeof(*uint_ptr);
    case 22: /* icmpOutEchoReps */
      *uint_ptr = STATS_GET(mib2.icmpoutechoreps);
      return sizeof(*uint_ptr);
    case 23: /* icmpOutTimestamps: not supported -> always 0 */
      *uint_ptr = 0;
      return sizeof(*uint_ptr);
    case 24: /* icmpOutTimestampReps: not supported -> always 0 */
      *uint_ptr = 0;
      return sizeof(*uint_ptr);
    case 25: /* icmpOutAddrMasks: not supported -> always 0 */
      *uint_ptr = 0;
      return sizeof(*uint_ptr);
    case 26: /* icmpOutAddrMaskReps: not supported -> always 0 */
      *uint_ptr = 0;
      return sizeof(*uint_ptr);
    default:
      LWIP_DEBUGF(SNMP_MIB_DEBUG, ("icmp_get_value(): unknown id: %"S32_F"\n", node->oid));
      break;
  }

  return 0;
}


static const struct snmp_scalar_array_node_def icmp_nodes[] = {
  { 1, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  { 2, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  { 3, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  { 4, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  { 5, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  { 6, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  { 7, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  { 8, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  { 9, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {10, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {11, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {12, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {13, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {14, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {15, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {16, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {17, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {18, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {19, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {20, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {21, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {22, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {23, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {24, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {25, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY},
  {26, SNMP_ASN1_TYPE_COUNTER, SNMP_NODE_INSTANCE_READ_ONLY}
};

const struct snmp_scalar_array_node snmp_mib2_icmp_root = SNMP_SCALAR_CREATE_ARRAY_NODE(5, icmp_nodes, icmp_get_value, NULL, NULL);

#endif /* LWIP_SNMP && SNMP_LWIP_MIB2 && LWIP_ICMP */
