
#ifndef LWIP_HDR_APPS_NETBIOS_H
#define LWIP_HDR_APPS_NETBIOS_H

#include "lwip/apps/netbiosns_opts.h"

#ifdef __cplusplus
extern "C" {
#endif

void netbiosns_init(void);
#ifndef NETBIOS_LWIP_NAME
void netbiosns_set_name(const char* hostname);
#endif
void netbiosns_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_NETBIOS_H */
