
#ifndef LWIP_HDR_TCPBASE_H
#define LWIP_HDR_TCPBASE_H

#include "lwip/opt.h"

#if LWIP_TCP /* don't build if not configured for use in lwipopts.h */

#ifdef __cplusplus
extern "C" {
#endif


#if LWIP_WND_SCALE
typedef u32_t tcpwnd_size_t;
#else
typedef u16_t tcpwnd_size_t;
#endif

enum tcp_state {
  CLOSED      = 0,
  LISTEN      = 1,
  SYN_SENT    = 2,
  SYN_RCVD    = 3,
  ESTABLISHED = 4,
  FIN_WAIT_1  = 5,
  FIN_WAIT_2  = 6,
  CLOSE_WAIT  = 7,
  CLOSING     = 8,
  LAST_ACK    = 9,
  TIME_WAIT   = 10
};
/* ATTENTION: this depends on state number ordering! */
#define TCP_STATE_IS_CLOSING(state) ((state) >= FIN_WAIT_1)

/* Flags for "apiflags" parameter in tcp_write */
#define TCP_WRITE_FLAG_COPY 0x01
#define TCP_WRITE_FLAG_MORE 0x02

#define TCP_PRIO_MIN    1
#define TCP_PRIO_NORMAL 64
#define TCP_PRIO_MAX    127

const char* tcp_debug_state_str(enum tcp_state s);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_TCP */

#endif /* LWIP_HDR_TCPBASE_H */
