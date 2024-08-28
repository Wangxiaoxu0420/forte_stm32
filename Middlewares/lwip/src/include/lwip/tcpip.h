
#ifndef LWIP_HDR_TCPIP_H
#define LWIP_HDR_TCPIP_H

#include "lwip/opt.h"

#if !NO_SYS /* don't build if not configured for use in lwipopts.h */

#include "lwip/err.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_TCPIP_CORE_LOCKING
/** The global semaphore to lock the stack. */
extern sys_mutex_t lock_tcpip_core;
#if !defined LOCK_TCPIP_CORE || defined __DOXYGEN__
/** Lock lwIP core mutex (needs @ref LWIP_TCPIP_CORE_LOCKING 1) */
#define LOCK_TCPIP_CORE()     sys_mutex_lock(&lock_tcpip_core)
/** Unlock lwIP core mutex (needs @ref LWIP_TCPIP_CORE_LOCKING 1) */
#define UNLOCK_TCPIP_CORE()   sys_mutex_unlock(&lock_tcpip_core)
#endif /* LOCK_TCPIP_CORE */
#else /* LWIP_TCPIP_CORE_LOCKING */
#define LOCK_TCPIP_CORE()
#define UNLOCK_TCPIP_CORE()
#endif /* LWIP_TCPIP_CORE_LOCKING */

struct pbuf;
struct netif;

/** Function prototype for the init_done function passed to tcpip_init */
typedef void (*tcpip_init_done_fn)(void *arg);
/** Function prototype for functions passed to tcpip_callback() */
typedef void (*tcpip_callback_fn)(void *ctx);

/* Forward declarations */
struct tcpip_callback_msg;

void   tcpip_init(tcpip_init_done_fn tcpip_init_done, void *arg);

err_t  tcpip_inpkt(struct pbuf *p, struct netif *inp, netif_input_fn input_fn);
err_t  tcpip_input(struct pbuf *p, struct netif *inp);

err_t  tcpip_try_callback(tcpip_callback_fn function, void *ctx);
err_t  tcpip_callback(tcpip_callback_fn function, void *ctx);
/**  @ingroup lwip_os
 * @deprecated use tcpip_try_callback() or tcpip_callback() instead
 */
#define tcpip_callback_with_block(function, ctx, block) ((block != 0)? tcpip_callback(function, ctx) : tcpip_try_callback(function, ctx))

struct tcpip_callback_msg* tcpip_callbackmsg_new(tcpip_callback_fn function, void *ctx);
void   tcpip_callbackmsg_delete(struct tcpip_callback_msg* msg);
err_t  tcpip_callbackmsg_trycallback(struct tcpip_callback_msg* msg);
err_t  tcpip_callbackmsg_trycallback_fromisr(struct tcpip_callback_msg* msg);

/* free pbufs or heap memory from another context without blocking */
err_t  pbuf_free_callback(struct pbuf *p);
err_t  mem_free_callback(void *m);

#if LWIP_TCPIP_TIMEOUT && LWIP_TIMERS
err_t  tcpip_timeout(u32_t msecs, sys_timeout_handler h, void *arg);
err_t  tcpip_untimeout(sys_timeout_handler h, void *arg);
#endif /* LWIP_TCPIP_TIMEOUT && LWIP_TIMERS */

#ifdef TCPIP_THREAD_TEST
int tcpip_thread_poll_one(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* !NO_SYS */

#endif /* LWIP_HDR_TCPIP_H */
