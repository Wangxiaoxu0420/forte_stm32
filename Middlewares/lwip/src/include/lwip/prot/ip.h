
#ifndef LWIP_HDR_PROT_IP_H
#define LWIP_HDR_PROT_IP_H

#include "lwip/arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IP_PROTO_ICMP    1
#define IP_PROTO_IGMP    2
#define IP_PROTO_UDP     17
#define IP_PROTO_UDPLITE 136
#define IP_PROTO_TCP     6

/** This operates on a void* by loading the first byte */
#define IP_HDR_GET_VERSION(ptr)   ((*(u8_t*)(ptr)) >> 4)

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_PROT_IP_H */
