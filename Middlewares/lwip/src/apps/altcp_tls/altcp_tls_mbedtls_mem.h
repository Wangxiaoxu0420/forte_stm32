
#ifndef LWIP_HDR_ALTCP_MBEDTLS_MEM_H
#define LWIP_HDR_ALTCP_MBEDTLS_MEM_H

#include "lwip/opt.h"

#if LWIP_ALTCP /* don't build if not configured for use in lwipopts.h */

#include "lwip/apps/altcp_tls_mbedtls_opts.h"

#if LWIP_ALTCP_TLS && LWIP_ALTCP_TLS_MBEDTLS

#include "altcp_tls_mbedtls_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

void altcp_mbedtls_mem_init(void);
altcp_mbedtls_state_t *altcp_mbedtls_alloc(void *conf);
void altcp_mbedtls_free(void *conf, altcp_mbedtls_state_t *state);
void *altcp_mbedtls_alloc_config(size_t size);
void altcp_mbedtls_free_config(void *item);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_ALTCP_TLS && LWIP_ALTCP_TLS_MBEDTLS */
#endif /* LWIP_ALTCP */
#endif /* LWIP_HDR_ALTCP_MBEDTLS_MEM_H */
