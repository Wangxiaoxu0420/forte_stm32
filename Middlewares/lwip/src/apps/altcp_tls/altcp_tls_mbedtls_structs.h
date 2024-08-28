
#ifndef LWIP_HDR_ALTCP_MBEDTLS_STRUCTS_H
#define LWIP_HDR_ALTCP_MBEDTLS_STRUCTS_H

#include "lwip/opt.h"

#if LWIP_ALTCP /* don't build if not configured for use in lwipopts.h */

#include "lwip/apps/altcp_tls_mbedtls_opts.h"

#if LWIP_ALTCP_TLS && LWIP_ALTCP_TLS_MBEDTLS

#include "lwip/altcp.h"
#include "lwip/pbuf.h"

#include "mbedtls/ssl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALTCP_MBEDTLS_FLAGS_HANDSHAKE_DONE    0x01
#define ALTCP_MBEDTLS_FLAGS_UPPER_CALLED      0x02
#define ALTCP_MBEDTLS_FLAGS_RX_CLOSE_QUEUED   0x04
#define ALTCP_MBEDTLS_FLAGS_RX_CLOSED         0x08
#define ALTCP_MBEDTLS_FLAGS_APPLDATA_SENT     0x10

typedef struct altcp_mbedtls_state_s {
  void *conf;
  mbedtls_ssl_context ssl_context;
  /* chain of rx pbufs (before decryption) */
  struct pbuf *rx;
  struct pbuf *rx_app;
  u8_t flags;
  int rx_passed_unrecved;
  int bio_bytes_read;
  int bio_bytes_appl;
} altcp_mbedtls_state_t;

#ifdef __cplusplus
}
#endif

#endif /* LWIP_ALTCP_TLS && LWIP_ALTCP_TLS_MBEDTLS */
#endif /* LWIP_ALTCP */
#endif /* LWIP_HDR_ALTCP_MBEDTLS_STRUCTS_H */
