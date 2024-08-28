

#ifndef LWIP_HDR_MEM_PRIV_H
#define LWIP_HDR_MEM_PRIV_H

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/mem.h"

#if MEM_OVERFLOW_CHECK || MEMP_OVERFLOW_CHECK
/* if MEM_OVERFLOW_CHECK or MEMP_OVERFLOW_CHECK is turned on, we reserve some
 * bytes at the beginning and at the end of each element, initialize them as
 * 0xcd and check them later.
 * If MEM(P)_OVERFLOW_CHECK is >= 2, on every call to mem(p)_malloc or mem(p)_free,
 * every single element in each pool/heap is checked!
 * This is VERY SLOW but also very helpful.
 * MEM_SANITY_REGION_BEFORE and MEM_SANITY_REGION_AFTER can be overridden in
 * lwipopts.h to change the amount reserved for checking. */
#ifndef MEM_SANITY_REGION_BEFORE
#define MEM_SANITY_REGION_BEFORE  16
#endif /* MEM_SANITY_REGION_BEFORE*/
#if MEM_SANITY_REGION_BEFORE > 0
#define MEM_SANITY_REGION_BEFORE_ALIGNED    LWIP_MEM_ALIGN_SIZE(MEM_SANITY_REGION_BEFORE)
#else
#define MEM_SANITY_REGION_BEFORE_ALIGNED    0
#endif /* MEM_SANITY_REGION_BEFORE*/
#ifndef MEM_SANITY_REGION_AFTER
#define MEM_SANITY_REGION_AFTER   16
#endif /* MEM_SANITY_REGION_AFTER*/
#if MEM_SANITY_REGION_AFTER > 0
#define MEM_SANITY_REGION_AFTER_ALIGNED     LWIP_MEM_ALIGN_SIZE(MEM_SANITY_REGION_AFTER)
#else
#define MEM_SANITY_REGION_AFTER_ALIGNED     0
#endif /* MEM_SANITY_REGION_AFTER*/

void mem_overflow_init_raw(void *p, size_t size);
void mem_overflow_check_raw(void *p, size_t size, const char *descr1, const char *descr2);

#endif /* MEM_OVERFLOW_CHECK || MEMP_OVERFLOW_CHECK */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_MEMP_PRIV_H */
