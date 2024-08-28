

#include "lwip/opt.h"

#if LWIP_ALTCP /* don't build if not configured for use in lwipopts.h */

#include "lwip/altcp.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/priv/altcp_priv.h"
#include "lwip/mem.h"

#include <string.h>

#if LWIP_ALTCP_TLS

/** This standard allocator function creates an altcp pcb for
 * TLS over TCP */
struct altcp_pcb *
altcp_tls_new(struct altcp_tls_config *config, u8_t ip_type)
{
  struct altcp_pcb *inner_conn, *ret;
  LWIP_UNUSED_ARG(ip_type);

  inner_conn = altcp_tcp_new_ip_type(ip_type);
  if (inner_conn == NULL) {
    return NULL;
  }
  ret = altcp_tls_wrap(config, inner_conn);
  if (ret == NULL) {
    altcp_close(inner_conn);
  }
  return ret;
}

/** This standard allocator function creates an altcp pcb for
 * TLS over TCP */
struct altcp_pcb *
altcp_tls_alloc(void *arg, u8_t ip_type)
{
  return altcp_tls_new((struct altcp_tls_config *)arg, ip_type);
}

#endif /* LWIP_ALTCP_TLS */

#endif /* LWIP_ALTCP */
