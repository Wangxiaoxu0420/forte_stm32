
#ifndef LWIP_HDR_PROT_MLD6_H
#define LWIP_HDR_PROT_MLD6_H

#include "lwip/arch.h"
#include "lwip/prot/ip6.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MLD6_HBH_HLEN 8
/** Multicast listener report/query/done message header. */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct mld_header {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t code);
  PACK_STRUCT_FIELD(u16_t chksum);
  PACK_STRUCT_FIELD(u16_t max_resp_delay);
  PACK_STRUCT_FIELD(u16_t reserved);
  PACK_STRUCT_FLD_S(ip6_addr_p_t multicast_address);
  /* Options follow. */
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_PROT_MLD6_H */
