
#ifndef LWIP_HDR_NETIF_SLIPIF_H
#define LWIP_HDR_NETIF_SLIPIF_H

#include "lwip/opt.h"
#include "lwip/netif.h"

/** Set this to 1 to start a thread that blocks reading on the serial line
 * (using sio_read()).
 */
#ifndef SLIP_USE_RX_THREAD
#define SLIP_USE_RX_THREAD !NO_SYS
#endif

/** Set this to 1 to enable functions to pass in RX bytes from ISR context.
 * If enabled, slipif_received_byte[s]() process incoming bytes and put assembled
 * packets on a queue, which is fed into lwIP from slipif_poll().
 * If disabled, slipif_poll() polls the serial line (using sio_tryread()).
 */
#ifndef SLIP_RX_FROM_ISR
#define SLIP_RX_FROM_ISR 0
#endif

/** Set this to 1 (default for SLIP_RX_FROM_ISR) to queue incoming packets
 * received by slipif_received_byte[s]() as long as PBUF_POOL pbufs are available.
 * If disabled, packets will be dropped if more than one packet is received.
 */
#ifndef SLIP_RX_QUEUE
#define SLIP_RX_QUEUE SLIP_RX_FROM_ISR
#endif

#ifdef __cplusplus
extern "C" {
#endif

err_t slipif_init(struct netif * netif);
void slipif_poll(struct netif *netif);
#if SLIP_RX_FROM_ISR
void slipif_process_rxqueue(struct netif *netif);
void slipif_received_byte(struct netif *netif, u8_t data);
void slipif_received_bytes(struct netif *netif, u8_t *data, u8_t len);
#endif /* SLIP_RX_FROM_ISR */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_NETIF_SLIPIF_H */

