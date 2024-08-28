

#include "lwip/apps/snmp_opts.h"
#include "lwip/ip_addr.h"

#if LWIP_SNMP && SNMP_USE_RAW

#include "lwip/udp.h"
#include "lwip/ip.h"
#include "lwip/prot/iana.h"
#include "snmp_msg.h"

/* lwIP UDP receive callback function */
static void
snmp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  LWIP_UNUSED_ARG(arg);

  snmp_receive(pcb, p, addr, port);

  pbuf_free(p);
}

err_t
snmp_sendto(void *handle, struct pbuf *p, const ip_addr_t *dst, u16_t port)
{
  return udp_sendto((struct udp_pcb *)handle, p, dst, port);
}

u8_t
snmp_get_local_ip_for_dst(void *handle, const ip_addr_t *dst, ip_addr_t *result)
{
  struct udp_pcb *udp_pcb = (struct udp_pcb *)handle;
  struct netif *dst_if;
  const ip_addr_t *dst_ip;

  LWIP_UNUSED_ARG(udp_pcb); /* unused in case of IPV4 only configuration */

  ip_route_get_local_ip(&udp_pcb->local_ip, dst, dst_if, dst_ip);

  if ((dst_if != NULL) && (dst_ip != NULL)) {
    ip_addr_copy(*result, *dst_ip);
    return 1;
  } else {
    return 0;
  }
}

/**
 * @ingroup snmp_core
 * Starts SNMP Agent.
 * Allocates UDP pcb and binds it to IP_ANY_TYPE port 161.
 */
void
snmp_init(void)
{
  err_t err;

  struct udp_pcb *snmp_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  LWIP_ERROR("snmp_raw: no PCB", (snmp_pcb != NULL), return;);

  LWIP_ASSERT_CORE_LOCKED();

  snmp_traps_handle = snmp_pcb;

  udp_recv(snmp_pcb, snmp_recv, NULL);
  err = udp_bind(snmp_pcb, IP_ANY_TYPE, LWIP_IANA_PORT_SNMP);
  LWIP_ERROR("snmp_raw: Unable to bind PCB", (err == ERR_OK), return;);
}

#endif /* LWIP_SNMP && SNMP_USE_RAW */
