

#ifndef LWIP_HDR_APPS_TFTP_OPTS_H
#define LWIP_HDR_APPS_TFTP_OPTS_H

#include "lwip/opt.h"
#include "lwip/prot/iana.h"

/**
 * @defgroup tftp_opts Options
 * @ingroup tftp
 * @{
 */

/**
 * Enable TFTP debug messages
 */
#if !defined TFTP_DEBUG || defined __DOXYGEN__
#define TFTP_DEBUG            LWIP_DBG_OFF
#endif

/**
 * TFTP server port
 */
#if !defined TFTP_PORT || defined __DOXYGEN__
#define TFTP_PORT             LWIP_IANA_PORT_TFTP
#endif

/**
 * TFTP timeout
 */
#if !defined TFTP_TIMEOUT_MSECS || defined __DOXYGEN__
#define TFTP_TIMEOUT_MSECS    10000
#endif

/**
 * Max. number of retries when a file is read from server
 */
#if !defined TFTP_MAX_RETRIES || defined __DOXYGEN__
#define TFTP_MAX_RETRIES      5
#endif

/**
 * TFTP timer cyclic interval
 */
#if !defined TFTP_TIMER_MSECS || defined __DOXYGEN__
#define TFTP_TIMER_MSECS      (TFTP_TIMEOUT_MSECS / 10)
#endif

/**
 * Max. length of TFTP filename
 */
#if !defined TFTP_MAX_FILENAME_LEN || defined __DOXYGEN__
#define TFTP_MAX_FILENAME_LEN 20
#endif

/**
 * Max. length of TFTP mode
 */
#if !defined TFTP_MAX_MODE_LEN || defined __DOXYGEN__
#define TFTP_MAX_MODE_LEN     7
#endif

/**
 * @}
 */

#endif /* LWIP_HDR_APPS_TFTP_OPTS_H */
