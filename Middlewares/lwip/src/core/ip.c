

#include "lwip/opt.h"

#if LWIP_IPV4 || LWIP_IPV6

#include "lwip/ip_addr.h"
#include "lwip/ip.h"

/** Global data for both IPv4 and IPv6 */
struct ip_globals ip_data;

#if LWIP_IPV4 && LWIP_IPV6

const ip_addr_t ip_addr_any_type = IPADDR_ANY_TYPE_INIT;

/**
 * @ingroup ipaddr
 * Convert numeric IP address (both versions) into ASCII representation.
 * returns ptr to static buffer; not reentrant!
 *
 * @param addr ip address in network order to convert
 * @return pointer to a global static (!) buffer that holds the ASCII
 *         representation of addr
 */
char *ipaddr_ntoa(const ip_addr_t *addr)
{
  if (addr == NULL) {
    return NULL;
  }
  if (IP_IS_V6(addr)) {
    return ip6addr_ntoa(ip_2_ip6(addr));
  } else {
    return ip4addr_ntoa(ip_2_ip4(addr));
  }
}

/**
 * @ingroup ipaddr
 * Same as ipaddr_ntoa, but reentrant since a user-supplied buffer is used.
 *
 * @param addr ip address in network order to convert
 * @param buf target buffer where the string is stored
 * @param buflen length of buf
 * @return either pointer to buf which now holds the ASCII
 *         representation of addr or NULL if buf was too small
 */
char *ipaddr_ntoa_r(const ip_addr_t *addr, char *buf, int buflen)
{
  if (addr == NULL) {
    return NULL;
  }
  if (IP_IS_V6(addr)) {
    return ip6addr_ntoa_r(ip_2_ip6(addr), buf, buflen);
  } else {
    return ip4addr_ntoa_r(ip_2_ip4(addr), buf, buflen);
  }
}

/**
 * @ingroup ipaddr
 * Convert IP address string (both versions) to numeric.
 * The version is auto-detected from the string.
 *
 * @param cp IP address string to convert
 * @param addr conversion result is stored here
 * @return 1 on success, 0 on error
 */
int
ipaddr_aton(const char *cp, ip_addr_t *addr)
{
  if (cp != NULL) {
    const char *c;
    for (c = cp; *c != 0; c++) {
      if (*c == ':') {
        /* contains a colon: IPv6 address */
        if (addr) {
          IP_SET_TYPE_VAL(*addr, IPADDR_TYPE_V6);
        }
        return ip6addr_aton(cp, ip_2_ip6(addr));
      } else if (*c == '.') {
        /* contains a dot: IPv4 address */
        break;
      }
    }
    /* call ip4addr_aton as fallback or if IPv4 was found */
    if (addr) {
      IP_SET_TYPE_VAL(*addr, IPADDR_TYPE_V4);
    }
    return ip4addr_aton(cp, ip_2_ip4(addr));
  }
  return 0;
}

/**
 * @ingroup lwip_nosys
 * If both IP versions are enabled, this function can dispatch packets to the correct one.
 * Don't call directly, pass to netif_add() and call netif->input().
 */
err_t
ip_input(struct pbuf *p, struct netif *inp)
{
  if (p != NULL) {
    if (IP_HDR_GET_VERSION(p->payload) == 6) {
      return ip6_input(p, inp);
    }
    return ip4_input(p, inp);
  }
  return ERR_VAL;
}

#endif /* LWIP_IPV4 && LWIP_IPV6 */

#endif /* LWIP_IPV4 || LWIP_IPV6 */
