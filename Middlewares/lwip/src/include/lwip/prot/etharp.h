
#ifndef LWIP_HDR_PROT_ETHARP_H
#define LWIP_HDR_PROT_ETHARP_H

#include "lwip/arch.h"
#include "lwip/prot/ethernet.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ETHARP_HWADDR_LEN
#define ETHARP_HWADDR_LEN     ETH_HWADDR_LEN
#endif

/**
 * struct ip4_addr_wordaligned is used in the definition of the ARP packet format in
 * order to support compilers that don't have structure packing.
 */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct ip4_addr_wordaligned {
  PACK_STRUCT_FIELD(u16_t addrw[2]);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** MEMCPY-like copying of IP addresses where addresses are known to be
 * 16-bit-aligned if the port is correctly configured (so a port could define
 * this to copying 2 u16_t's) - no NULL-pointer-checking needed. */
#ifndef IPADDR_WORDALIGNED_COPY_TO_IP4_ADDR_T
#define IPADDR_WORDALIGNED_COPY_TO_IP4_ADDR_T(dest, src) SMEMCPY(dest, src, sizeof(ip4_addr_t))
#endif

 /** MEMCPY-like copying of IP addresses where addresses are known to be
 * 16-bit-aligned if the port is correctly configured (so a port could define
 * this to copying 2 u16_t's) - no NULL-pointer-checking needed. */
#ifndef IPADDR_WORDALIGNED_COPY_FROM_IP4_ADDR_T
#define IPADDR_WORDALIGNED_COPY_FROM_IP4_ADDR_T(dest, src) SMEMCPY(dest, src, sizeof(ip4_addr_t))
#endif

#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
/** the ARP message, see RFC 826 ("Packet format") */
struct etharp_hdr {
  PACK_STRUCT_FIELD(u16_t hwtype);
  PACK_STRUCT_FIELD(u16_t proto);
  PACK_STRUCT_FLD_8(u8_t  hwlen);
  PACK_STRUCT_FLD_8(u8_t  protolen);
  PACK_STRUCT_FIELD(u16_t opcode);
  PACK_STRUCT_FLD_S(struct eth_addr shwaddr);
  PACK_STRUCT_FLD_S(struct ip4_addr_wordaligned sipaddr);
  PACK_STRUCT_FLD_S(struct eth_addr dhwaddr);
  PACK_STRUCT_FLD_S(struct ip4_addr_wordaligned dipaddr);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#define SIZEOF_ETHARP_HDR 28

/* ARP message types (opcodes) */
enum etharp_opcode {
  ARP_REQUEST = 1,
  ARP_REPLY   = 2
};

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_PROT_ETHARP_H */
