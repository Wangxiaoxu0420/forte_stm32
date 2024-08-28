
#include "lwip/opt.h"

#if LWIP_SOCKET

#include "lwip/errno.h"
#include "lwip/if_api.h"
#include "lwip/netifapi.h"
#include "lwip/priv/sockets_priv.h"

/**
 * @ingroup if_api
 * Maps an interface index to its corresponding name.
 * @param ifindex interface index
 * @param ifname shall point to a buffer of at least {IF_NAMESIZE} bytes
 * @return If ifindex is an interface index, then the function shall return the
 * value supplied in ifname, which points to a buffer now containing the interface name.
 * Otherwise, the function shall return a NULL pointer.
 */
char *
lwip_if_indextoname(unsigned int ifindex, char *ifname)
{
#if LWIP_NETIF_API
  if (ifindex <= 0xff) {
    err_t err = netifapi_netif_index_to_name((u8_t)ifindex, ifname);
    if (!err && ifname[0] != '\0') {
      return ifname;
    }
  }
#else /* LWIP_NETIF_API */
  LWIP_UNUSED_ARG(ifindex);
  LWIP_UNUSED_ARG(ifname);
#endif /* LWIP_NETIF_API */
  set_errno(ENXIO);
  return NULL;
}

/**
 * @ingroup if_api
 * Returs the interface index corresponding to name ifname.
 * @param ifname Interface name
 * @return The corresponding index if ifname is the name of an interface;
 * otherwise, zero.
 */
unsigned int
lwip_if_nametoindex(const char *ifname)
{
#if LWIP_NETIF_API
  err_t err;
  u8_t idx;

  err = netifapi_netif_name_to_index(ifname, &idx);
  if (!err) {
    return idx;
  }
#else /* LWIP_NETIF_API */
  LWIP_UNUSED_ARG(ifname);
#endif /* LWIP_NETIF_API */
  return 0; /* invalid index */
}

#endif /* LWIP_SOCKET */
