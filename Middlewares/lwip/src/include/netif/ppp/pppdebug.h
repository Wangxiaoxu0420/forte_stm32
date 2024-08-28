
#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT /* don't build if not configured for use in lwipopts.h */

#ifndef PPPDEBUG_H
#define PPPDEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Trace levels. */
#define LOG_CRITICAL  (PPP_DEBUG | LWIP_DBG_LEVEL_SEVERE)
#define LOG_ERR       (PPP_DEBUG | LWIP_DBG_LEVEL_SEVERE)
#define LOG_NOTICE    (PPP_DEBUG | LWIP_DBG_LEVEL_WARNING)
#define LOG_WARNING   (PPP_DEBUG | LWIP_DBG_LEVEL_WARNING)
#define LOG_INFO      (PPP_DEBUG)
#define LOG_DETAIL    (PPP_DEBUG)
#define LOG_DEBUG     (PPP_DEBUG)

#if PPP_DEBUG

#define MAINDEBUG(a)    LWIP_DEBUGF(LWIP_DBG_LEVEL_WARNING, a)
#define SYSDEBUG(a)     LWIP_DEBUGF(LWIP_DBG_LEVEL_WARNING, a)
#define FSMDEBUG(a)     LWIP_DEBUGF(LWIP_DBG_LEVEL_WARNING, a)
#define LCPDEBUG(a)     LWIP_DEBUGF(LWIP_DBG_LEVEL_WARNING, a)
#define IPCPDEBUG(a)    LWIP_DEBUGF(LWIP_DBG_LEVEL_WARNING, a)
#define IPV6CPDEBUG(a)  LWIP_DEBUGF(LWIP_DBG_LEVEL_WARNING, a)
#define UPAPDEBUG(a)    LWIP_DEBUGF(LWIP_DBG_LEVEL_WARNING, a)
#define CHAPDEBUG(a)    LWIP_DEBUGF(LWIP_DBG_LEVEL_WARNING, a)
#define PPPDEBUG(a, b)  LWIP_DEBUGF(a, b)

#else /* PPP_DEBUG */

#define MAINDEBUG(a)
#define SYSDEBUG(a)
#define FSMDEBUG(a)
#define LCPDEBUG(a)
#define IPCPDEBUG(a)
#define IPV6CPDEBUG(a)
#define UPAPDEBUG(a)
#define CHAPDEBUG(a)
#define PPPDEBUG(a, b)

#endif /* PPP_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* PPPDEBUG_H */

#endif /* PPP_SUPPORT */
