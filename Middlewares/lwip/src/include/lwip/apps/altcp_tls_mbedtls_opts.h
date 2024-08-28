
#ifndef LWIP_HDR_ALTCP_TLS_OPTS_H
#define LWIP_HDR_ALTCP_TLS_OPTS_H

#include "lwip/opt.h"

#if LWIP_ALTCP /* don't build if not configured for use in lwipopts.h */

/** LWIP_ALTCP_TLS_MBEDTLS==1: use mbedTLS for TLS support for altcp API
 * mbedtls include directory must be reachable via include search path
 */
#ifndef LWIP_ALTCP_TLS_MBEDTLS
#define LWIP_ALTCP_TLS_MBEDTLS                        0
#endif

/** Configure debug level of this file */
#ifndef ALTCP_MBEDTLS_DEBUG
#define ALTCP_MBEDTLS_DEBUG                           LWIP_DBG_OFF
#endif

/** Configure lwIP debug level of the mbedTLS library */
#ifndef ALTCP_MBEDTLS_LIB_DEBUG
#define ALTCP_MBEDTLS_LIB_DEBUG                       LWIP_DBG_OFF
#endif

/** Configure minimum internal debug level of the mbedTLS library */
#ifndef ALTCP_MBEDTLS_LIB_DEBUG_LEVEL_MIN
#define ALTCP_MBEDTLS_LIB_DEBUG_LEVEL_MIN             0
#endif

/** Enable the basic session cache
 * ATTENTION: Using a session cache can lower security by reusing keys!
 */
#ifndef ALTCP_MBEDTLS_USE_SESSION_CACHE
#define ALTCP_MBEDTLS_USE_SESSION_CACHE               0
#endif

/** Maximum cache size of the basic session cache */
#ifndef ALTCP_MBEDTLS_SESSION_CACHE_SIZE
#define ALTCP_MBEDTLS_SESSION_CACHE_SIZE              30
#endif

/** Set a session timeout in seconds for the basic session cache  */
#ifndef ALTCP_MBEDTLS_SESSION_CACHE_TIMEOUT_SECONDS
#define ALTCP_MBEDTLS_SESSION_CACHE_TIMEOUT_SECONDS   (60 * 60)
#endif

/** Use session tickets to speed up connection setup (needs
 * MBEDTLS_SSL_SESSION_TICKETS enabled in mbedTLS config).
 * ATTENTION: Using session tickets can lower security by reusing keys!
 */
#ifndef ALTCP_MBEDTLS_USE_SESSION_TICKETS
#define ALTCP_MBEDTLS_USE_SESSION_TICKETS             0
#endif

/** Session ticket cipher */
#ifndef ALTCP_MBEDTLS_SESSION_TICKET_CIPHER
#define ALTCP_MBEDTLS_SESSION_TICKET_CIPHER           MBEDTLS_CIPHER_AES_256_GCM
#endif

/** Maximum timeout for session tickets */
#ifndef ALTCP_MBEDTLS_SESSION_TICKET_TIMEOUT_SECONDS
#define ALTCP_MBEDTLS_SESSION_TICKET_TIMEOUT_SECONDS  (60 * 60 * 24)
#endif

#endif /* LWIP_ALTCP */

#endif /* LWIP_HDR_ALTCP_TLS_OPTS_H */
