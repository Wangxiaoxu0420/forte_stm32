
#ifndef LWIP_HDR_TCPIP_PRIV_H
#define LWIP_HDR_TCPIP_PRIV_H

#include "lwip/opt.h"

#if !NO_SYS /* don't build if not configured for use in lwipopts.h */

#include "lwip/tcpip.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"

#ifdef __cplusplus
extern "C" {
#endif

struct pbuf;
struct netif;

#if LWIP_MPU_COMPATIBLE
#define API_VAR_REF(name)               (*(name))
#define API_VAR_DECLARE(type, name)     type * name
#define API_VAR_ALLOC_EXT(type, pool, name, errorblock) do { \
                                          name = (type *)memp_malloc(pool); \
                                          if (name == NULL) { \
                                            errorblock; \
                                          } \
                                        } while(0)
#define API_VAR_ALLOC(type, pool, name, errorval) API_VAR_ALLOC_EXT(type, pool, name, return errorval)
#define API_VAR_ALLOC_POOL(type, pool, name, errorval) do { \
                                          name = (type *)LWIP_MEMPOOL_ALLOC(pool); \
                                          if (name == NULL) { \
                                            return errorval; \
                                          } \
                                        } while(0)
#define API_VAR_FREE(pool, name)        memp_free(pool, name)
#define API_VAR_FREE_POOL(pool, name)   LWIP_MEMPOOL_FREE(pool, name)
#define API_EXPR_REF(expr)              (&(expr))
#if LWIP_NETCONN_SEM_PER_THREAD
#define API_EXPR_REF_SEM(expr)          (expr)
#else
#define API_EXPR_REF_SEM(expr)          API_EXPR_REF(expr)
#endif
#define API_EXPR_DEREF(expr)            expr
#define API_MSG_M_DEF(m)                m
#define API_MSG_M_DEF_C(t, m)           t m
#else /* LWIP_MPU_COMPATIBLE */
#define API_VAR_REF(name)               name
#define API_VAR_DECLARE(type, name)     type name
#define API_VAR_ALLOC_EXT(type, pool, name, errorblock)
#define API_VAR_ALLOC(type, pool, name, errorval)
#define API_VAR_ALLOC_POOL(type, pool, name, errorval)
#define API_VAR_FREE(pool, name)
#define API_VAR_FREE_POOL(pool, name)
#define API_EXPR_REF(expr)              expr
#define API_EXPR_REF_SEM(expr)          API_EXPR_REF(expr)
#define API_EXPR_DEREF(expr)            (*(expr))
#define API_MSG_M_DEF(m)                *m
#define API_MSG_M_DEF_C(t, m)           const t * m
#endif /* LWIP_MPU_COMPATIBLE */

err_t tcpip_send_msg_wait_sem(tcpip_callback_fn fn, void *apimsg, sys_sem_t* sem);

struct tcpip_api_call_data
{
#if !LWIP_TCPIP_CORE_LOCKING
  err_t err;
#if !LWIP_NETCONN_SEM_PER_THREAD
  sys_sem_t sem;
#endif /* LWIP_NETCONN_SEM_PER_THREAD */
#else /* !LWIP_TCPIP_CORE_LOCKING */
  u8_t dummy; /* avoid empty struct :-( */
#endif /* !LWIP_TCPIP_CORE_LOCKING */
};
typedef err_t (*tcpip_api_call_fn)(struct tcpip_api_call_data* call);
err_t tcpip_api_call(tcpip_api_call_fn fn, struct tcpip_api_call_data *call);

enum tcpip_msg_type {
#if !LWIP_TCPIP_CORE_LOCKING
  TCPIP_MSG_API,
  TCPIP_MSG_API_CALL,
#endif /* !LWIP_TCPIP_CORE_LOCKING */
#if !LWIP_TCPIP_CORE_LOCKING_INPUT
  TCPIP_MSG_INPKT,
#endif /* !LWIP_TCPIP_CORE_LOCKING_INPUT */
#if LWIP_TCPIP_TIMEOUT && LWIP_TIMERS
  TCPIP_MSG_TIMEOUT,
  TCPIP_MSG_UNTIMEOUT,
#endif /* LWIP_TCPIP_TIMEOUT && LWIP_TIMERS */
  TCPIP_MSG_CALLBACK,
  TCPIP_MSG_CALLBACK_STATIC
};

struct tcpip_msg {
  enum tcpip_msg_type type;
  union {
#if !LWIP_TCPIP_CORE_LOCKING
    struct {
      tcpip_callback_fn function;
      void* msg;
    } api_msg;
    struct {
      tcpip_api_call_fn function;
      struct tcpip_api_call_data *arg;
      sys_sem_t *sem;
    } api_call;
#endif /* LWIP_TCPIP_CORE_LOCKING */
#if !LWIP_TCPIP_CORE_LOCKING_INPUT
    struct {
      struct pbuf *p;
      struct netif *netif;
      netif_input_fn input_fn;
    } inp;
#endif /* !LWIP_TCPIP_CORE_LOCKING_INPUT */
    struct {
      tcpip_callback_fn function;
      void *ctx;
    } cb;
#if LWIP_TCPIP_TIMEOUT && LWIP_TIMERS
    struct {
      u32_t msecs;
      sys_timeout_handler h;
      void *arg;
    } tmo;
#endif /* LWIP_TCPIP_TIMEOUT && LWIP_TIMERS */
  } msg;
};

#ifdef __cplusplus
}
#endif

#endif /* !NO_SYS */

#endif /* LWIP_HDR_TCPIP_PRIV_H */
