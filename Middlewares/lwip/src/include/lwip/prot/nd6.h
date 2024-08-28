
#ifndef LWIP_HDR_PROT_ND6_H
#define LWIP_HDR_PROT_ND6_H

#include "lwip/arch.h"
#include "lwip/ip6_addr.h"
#include "lwip/prot/ip6.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Neighbor solicitation message header. */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct ns_header {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t code);
  PACK_STRUCT_FIELD(u16_t chksum);
  PACK_STRUCT_FIELD(u32_t reserved);
  PACK_STRUCT_FLD_S(ip6_addr_p_t target_address);
  /* Options follow. */
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** Neighbor advertisement message header. */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct na_header {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t code);
  PACK_STRUCT_FIELD(u16_t chksum);
  PACK_STRUCT_FLD_8(u8_t flags);
  PACK_STRUCT_FLD_8(u8_t reserved[3]);
  PACK_STRUCT_FLD_S(ip6_addr_p_t target_address);
  /* Options follow. */
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif
#define ND6_FLAG_ROUTER      (0x80)
#define ND6_FLAG_SOLICITED   (0x40)
#define ND6_FLAG_OVERRIDE    (0x20)

/** Router solicitation message header. */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct rs_header {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t code);
  PACK_STRUCT_FIELD(u16_t chksum);
  PACK_STRUCT_FIELD(u32_t reserved);
  /* Options follow. */
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** Router advertisement message header. */
#define ND6_RA_FLAG_MANAGED_ADDR_CONFIG (0x80)
#define ND6_RA_FLAG_OTHER_CONFIG (0x40)
#define ND6_RA_FLAG_HOME_AGENT (0x20)
#define ND6_RA_PREFERENCE_MASK (0x18)
#define ND6_RA_PREFERENCE_HIGH (0x08)
#define ND6_RA_PREFERENCE_MEDIUM (0x00)
#define ND6_RA_PREFERENCE_LOW (0x18)
#define ND6_RA_PREFERENCE_DISABLED (0x10)
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct ra_header {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t code);
  PACK_STRUCT_FIELD(u16_t chksum);
  PACK_STRUCT_FLD_8(u8_t current_hop_limit);
  PACK_STRUCT_FLD_8(u8_t flags);
  PACK_STRUCT_FIELD(u16_t router_lifetime);
  PACK_STRUCT_FIELD(u32_t reachable_time);
  PACK_STRUCT_FIELD(u32_t retrans_timer);
  /* Options follow. */
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** Redirect message header. */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct redirect_header {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t code);
  PACK_STRUCT_FIELD(u16_t chksum);
  PACK_STRUCT_FIELD(u32_t reserved);
  PACK_STRUCT_FLD_S(ip6_addr_p_t target_address);
  PACK_STRUCT_FLD_S(ip6_addr_p_t destination_address);
  /* Options follow. */
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** Link-layer address option. */
#define ND6_OPTION_TYPE_SOURCE_LLADDR (0x01)
#define ND6_OPTION_TYPE_TARGET_LLADDR (0x02)
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct lladdr_option {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t length);
  PACK_STRUCT_FLD_8(u8_t addr[NETIF_MAX_HWADDR_LEN]);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** Prefix information option. */
#define ND6_OPTION_TYPE_PREFIX_INFO (0x03)
#define ND6_PREFIX_FLAG_ON_LINK (0x80)
#define ND6_PREFIX_FLAG_AUTONOMOUS (0x40)
#define ND6_PREFIX_FLAG_ROUTER_ADDRESS (0x20)
#define ND6_PREFIX_FLAG_SITE_PREFIX (0x10)
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct prefix_option {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t length);
  PACK_STRUCT_FLD_8(u8_t prefix_length);
  PACK_STRUCT_FLD_8(u8_t flags);
  PACK_STRUCT_FIELD(u32_t valid_lifetime);
  PACK_STRUCT_FIELD(u32_t preferred_lifetime);
  PACK_STRUCT_FLD_8(u8_t reserved2[3]);
  PACK_STRUCT_FLD_8(u8_t site_prefix_length);
  PACK_STRUCT_FLD_S(ip6_addr_p_t prefix);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** Redirected header option. */
#define ND6_OPTION_TYPE_REDIR_HDR (0x04)
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct redirected_header_option {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t length);
  PACK_STRUCT_FLD_8(u8_t reserved[6]);
  /* Portion of redirected packet follows. */
  /* PACK_STRUCT_FLD_8(u8_t redirected[8]); */
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** MTU option. */
#define ND6_OPTION_TYPE_MTU (0x05)
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct mtu_option {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t length);
  PACK_STRUCT_FIELD(u16_t reserved);
  PACK_STRUCT_FIELD(u32_t mtu);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** Route information option. */
#define ND6_OPTION_TYPE_ROUTE_INFO (24)
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct route_option {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t length);
  PACK_STRUCT_FLD_8(u8_t prefix_length);
  PACK_STRUCT_FLD_8(u8_t preference);
  PACK_STRUCT_FIELD(u32_t route_lifetime);
  PACK_STRUCT_FLD_S(ip6_addr_p_t prefix);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** Recursive DNS Server Option. */
#define ND6_OPTION_TYPE_RDNSS (25)
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct rdnss_option {
  PACK_STRUCT_FLD_8(u8_t type);
  PACK_STRUCT_FLD_8(u8_t length);
  PACK_STRUCT_FIELD(u16_t reserved);
  PACK_STRUCT_FIELD(u32_t lifetime);
  PACK_STRUCT_FLD_S(ip6_addr_p_t rdnss_address[1]);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#define SIZEOF_RDNSS_OPTION_BASE 8 /* size without addresses */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_PROT_ND6_H */
