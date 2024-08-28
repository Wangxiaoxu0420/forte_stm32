
#ifndef LWIP_HDR_MDNS_PRIV_H
#define LWIP_HDR_MDNS_PRIV_H

#include "lwip/apps/mdns_opts.h"
#include "lwip/pbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_MDNS_RESPONDER

/* Domain struct and methods - visible for unit tests */

#define MDNS_DOMAIN_MAXLEN 256
#define MDNS_READNAME_ERROR 0xFFFF

struct mdns_domain {
  /* Encoded domain name */
  u8_t name[MDNS_DOMAIN_MAXLEN];
  /* Total length of domain name, including zero */
  u16_t length;
  /* Set if compression of this domain is not allowed */
  u8_t skip_compression;
};

err_t mdns_domain_add_label(struct mdns_domain *domain, const char *label, u8_t len);
u16_t mdns_readname(struct pbuf *p, u16_t offset, struct mdns_domain *domain);
int mdns_domain_eq(struct mdns_domain *a, struct mdns_domain *b);
u16_t mdns_compress_domain(struct pbuf *pbuf, u16_t *offset, struct mdns_domain *domain);

#endif /* LWIP_MDNS_RESPONDER */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_MDNS_PRIV_H */
