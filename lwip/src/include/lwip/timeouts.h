
#ifndef LWIP_HDR_TIMEOUTS_H
#define LWIP_HDR_TIMEOUTS_H

#include "lwip/opt.h"
#include "lwip/err.h"
#if !NO_SYS
#include "lwip/sys.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LWIP_DEBUG_TIMERNAMES
#ifdef LWIP_DEBUG
#define LWIP_DEBUG_TIMERNAMES SYS_DEBUG
#else /* LWIP_DEBUG */
#define LWIP_DEBUG_TIMERNAMES 0
#endif /* LWIP_DEBUG*/
#endif

/** Returned by sys_timeouts_sleeptime() to indicate there is no timer, so we
 * can sleep forever.
 */
#define SYS_TIMEOUTS_SLEEPTIME_INFINITE 0xFFFFFFFF

/** Function prototype for a stack-internal timer function that has to be
 * called at a defined interval */
typedef void (* lwip_cyclic_timer_handler)(void);

/** This struct contains information about a stack-internal timer function
 that has to be called at a defined interval */
struct lwip_cyclic_timer {
  u32_t interval_ms;
  lwip_cyclic_timer_handler handler;
#if LWIP_DEBUG_TIMERNAMES
  const char* handler_name;
#endif /* LWIP_DEBUG_TIMERNAMES */
};

/** This array contains all stack-internal cyclic timers. To get the number of
 * timers, use lwip_num_cyclic_timers */
extern const struct lwip_cyclic_timer lwip_cyclic_timers[];
/** Array size of lwip_cyclic_timers[] */
extern const int lwip_num_cyclic_timers;

#if LWIP_TIMERS

/** Function prototype for a timeout callback function. Register such a function
 * using sys_timeout().
 *
 * @param arg Additional argument to pass to the function - set up by sys_timeout()
 */
typedef void (* sys_timeout_handler)(void *arg);

struct sys_timeo {
  struct sys_timeo *next;
  u32_t time;
  sys_timeout_handler h;
  void *arg;
#if LWIP_DEBUG_TIMERNAMES
  const char* handler_name;
#endif /* LWIP_DEBUG_TIMERNAMES */
};

void sys_timeouts_init(void);

#if LWIP_DEBUG_TIMERNAMES
void sys_timeout_debug(u32_t msecs, sys_timeout_handler handler, void *arg, const char* handler_name);
#define sys_timeout(msecs, handler, arg) sys_timeout_debug(msecs, handler, arg, #handler)
#else /* LWIP_DEBUG_TIMERNAMES */
void sys_timeout(u32_t msecs, sys_timeout_handler handler, void *arg);
#endif /* LWIP_DEBUG_TIMERNAMES */

void sys_untimeout(sys_timeout_handler handler, void *arg);
void sys_restart_timeouts(void);
void sys_check_timeouts(void);
u32_t sys_timeouts_sleeptime(void);

#if LWIP_TESTMODE
struct sys_timeo** sys_timeouts_get_next_timeout(void);
void lwip_cyclic_timer(void *arg);
#endif

#endif /* LWIP_TIMERS */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_TIMEOUTS_H */
