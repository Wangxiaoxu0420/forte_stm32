

#include "lwip/apps/snmp_opts.h"

#if LWIP_SNMP && SNMP_USE_NETCONN

#include <string.h>
#include "lwip/api.h"
#include "lwip/ip.h"
#include "lwip/udp.h"
#include "snmp_msg.h"
#include "lwip/sys.h"
#include "lwip/prot/iana.h"

/** SNMP netconn API worker thread */
static void
snmp_netconn_thread(void *arg)
{
  struct netconn *conn;
  struct netbuf *buf;
  err_t err;
  LWIP_UNUSED_ARG(arg);

  /* Bind to SNMP port with default IP address */
#if LWIP_IPV6
  conn = netconn_new(NETCONN_UDP_IPV6);
  netconn_bind(conn, IP6_ADDR_ANY, LWIP_IANA_PORT_SNMP);
#else /* LWIP_IPV6 */
  conn = netconn_new(NETCONN_UDP);
  netconn_bind(conn, IP4_ADDR_ANY, LWIP_IANA_PORT_SNMP);
#endif /* LWIP_IPV6 */
  LWIP_ERROR("snmp_netconn: invalid conn", (conn != NULL), return;);

  snmp_traps_handle = conn;

  do {
    err = netconn_recv(conn, &buf);

    if (err == ERR_OK) {
      snmp_receive(conn, buf->p, &buf->addr, buf->port);
    }

    if (buf != NULL) {
      netbuf_delete(buf);
    }
  } while (1);
}

err_t
snmp_sendto(void *handle, struct pbuf *p, const ip_addr_t *dst, u16_t port)
{
  err_t result;
  struct netbuf buf;

  memset(&buf, 0, sizeof(buf));
  buf.p = p;
  result = netconn_sendto((struct netconn *)handle, &buf, dst, port);

  return result;
}

u8_t
snmp_get_local_ip_for_dst(void *handle, const ip_addr_t *dst, ip_addr_t *result)
{
  struct netconn *conn = (struct netconn *)handle;
  struct netif *dst_if;
  const ip_addr_t *dst_ip;

  LWIP_UNUSED_ARG(conn); /* unused in case of IPV4 only configuration */

  ip_route_get_local_ip(&conn->pcb.udp->local_ip, dst, dst_if, dst_ip);

  if ((dst_if != NULL) && (dst_ip != NULL)) {
    ip_addr_copy(*result, *dst_ip);
    return 1;
  } else {
    return 0;
  }
}

/**
 * Starts SNMP Agent.
 */
void
snmp_init(void)
{
  LWIP_ASSERT_CORE_LOCKED();
  sys_thread_new("snmp_netconn", snmp_netconn_thread, NULL, SNMP_STACK_SIZE, SNMP_THREAD_PRIO);
}

#endif /* LWIP_SNMP && SNMP_USE_NETCONN */
