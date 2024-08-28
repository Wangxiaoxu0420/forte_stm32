

#ifndef LWIP_HDR_NETIF_BRIDGEIF_OPTS_H
#define LWIP_HDR_NETIF_BRIDGEIF_OPTS_H

#include "lwip/opt.h"

/**
 * @defgroup bridgeif_opts Options
 * @ingroup bridgeif
 * @{
 */

/** BRIDGEIF_PORT_NETIFS_OUTPUT_DIRECT==1: set port netif's 'input' function
 * to call directly into bridgeif code and on top of that, directly call into
 * the selected forwarding port's 'linkoutput' function.
 * This means that the bridgeif input/output path is protected from concurrent access
 * but as well, *all* bridge port netif's drivers must correctly handle concurrent access!
 * == 0: get into tcpip_thread for every input packet (no multithreading)
 * ATTENTION: as ==0 relies on tcpip.h, the default depends on NO_SYS setting
 */
#ifndef BRIDGEIF_PORT_NETIFS_OUTPUT_DIRECT
#define BRIDGEIF_PORT_NETIFS_OUTPUT_DIRECT  NO_SYS
#endif

/** BRIDGEIF_MAX_PORTS: this is used to create a typedef used for forwarding
 * bit-fields: the number of bits required is this + 1 (for the internal/cpu port)
 * (63 is the maximum, resulting in an u64_t for the bit mask)
 * ATTENTION: this controls the maximum number of the implementation only!
 * The max. number of ports per bridge must still be passed via netif_add parameter!
 */
#ifndef BRIDGEIF_MAX_PORTS
#define BRIDGEIF_MAX_PORTS                  7
#endif

/** BRIDGEIF_DEBUG: Enable generic debugging in bridgeif.c. */
#ifndef BRIDGEIF_DEBUG
#define BRIDGEIF_DEBUG                      LWIP_DBG_OFF
#endif

/** BRIDGEIF_DEBUG: Enable FDB debugging in bridgeif.c. */
#ifndef BRIDGEIF_FDB_DEBUG
#define BRIDGEIF_FDB_DEBUG                  LWIP_DBG_OFF
#endif

/** BRIDGEIF_DEBUG: Enable forwarding debugging in bridgeif.c. */
#ifndef BRIDGEIF_FW_DEBUG
#define BRIDGEIF_FW_DEBUG                   LWIP_DBG_OFF
#endif

/**
 * @}
 */

#endif /* LWIP_HDR_NETIF_BRIDGEIF_OPTS_H */
