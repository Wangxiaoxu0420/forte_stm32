

#ifndef LWIP_HDR_MEMP_H
#define LWIP_HDR_MEMP_H

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* run once with empty definition to handle all custom includes in lwippools.h */
#define LWIP_MEMPOOL(name,num,size,desc)
#include "lwip/priv/memp_std.h"

/** Create the list of all memory pools managed by memp. MEMP_MAX represents a NULL pool at the end */
typedef enum {
#define LWIP_MEMPOOL(name,num,size,desc)  MEMP_##name,
#include "lwip/priv/memp_std.h"
  MEMP_MAX
} memp_t;

#include "lwip/priv/memp_priv.h"
#include "lwip/stats.h"

extern const struct memp_desc* const memp_pools[MEMP_MAX];

/**
 * @ingroup mempool
 * Declare prototype for private memory pool if it is used in multiple files
 */
#define LWIP_MEMPOOL_PROTOTYPE(name) extern const struct memp_desc memp_ ## name

#if MEMP_MEM_MALLOC

#define LWIP_MEMPOOL_DECLARE(name,num,size,desc) \
  LWIP_MEMPOOL_DECLARE_STATS_INSTANCE(memp_stats_ ## name) \
  const struct memp_desc memp_ ## name = { \
    DECLARE_LWIP_MEMPOOL_DESC(desc) \
    LWIP_MEMPOOL_DECLARE_STATS_REFERENCE(memp_stats_ ## name) \
    LWIP_MEM_ALIGN_SIZE(size) \
  };

#else /* MEMP_MEM_MALLOC */

/**
 * @ingroup mempool
 * Declare a private memory pool
 * Private mempools example:
 * .h: only when pool is used in multiple .c files: LWIP_MEMPOOL_PROTOTYPE(my_private_pool);
 * .c:
 *   - in global variables section: LWIP_MEMPOOL_DECLARE(my_private_pool, 10, sizeof(foo), "Some description")
 *   - call ONCE before using pool (e.g. in some init() function): LWIP_MEMPOOL_INIT(my_private_pool);
 *   - allocate: void* my_new_mem = LWIP_MEMPOOL_ALLOC(my_private_pool);
 *   - free: LWIP_MEMPOOL_FREE(my_private_pool, my_new_mem);
 *
 * To relocate a pool, declare it as extern in cc.h. Example for GCC:
 *   extern u8_t \_\_attribute\_\_((section(".onchip_mem"))) memp_memory_my_private_pool_base[];
 */
#define LWIP_MEMPOOL_DECLARE(name,num,size,desc) \
  LWIP_DECLARE_MEMORY_ALIGNED(memp_memory_ ## name ## _base, ((num) * (MEMP_SIZE + MEMP_ALIGN_SIZE(size)))); \
    \
  LWIP_MEMPOOL_DECLARE_STATS_INSTANCE(memp_stats_ ## name) \
    \
  static struct memp *memp_tab_ ## name; \
    \
  const struct memp_desc memp_ ## name = { \
    DECLARE_LWIP_MEMPOOL_DESC(desc) \
    LWIP_MEMPOOL_DECLARE_STATS_REFERENCE(memp_stats_ ## name) \
    LWIP_MEM_ALIGN_SIZE(size), \
    (num), \
    memp_memory_ ## name ## _base, \
    &memp_tab_ ## name \
  };

#endif /* MEMP_MEM_MALLOC */

/**
 * @ingroup mempool
 * Initialize a private memory pool
 */
#define LWIP_MEMPOOL_INIT(name)    memp_init_pool(&memp_ ## name)
/**
 * @ingroup mempool
 * Allocate from a private memory pool
 */
#define LWIP_MEMPOOL_ALLOC(name)   memp_malloc_pool(&memp_ ## name)
/**
 * @ingroup mempool
 * Free element from a private memory pool
 */
#define LWIP_MEMPOOL_FREE(name, x) memp_free_pool(&memp_ ## name, (x))

#if MEM_USE_POOLS
/** This structure is used to save the pool one element came from.
 * This has to be defined here as it is required for pool size calculation. */
struct memp_malloc_helper
{
   memp_t poolnr;
#if MEMP_OVERFLOW_CHECK || (LWIP_STATS && MEM_STATS)
   u16_t size;
#endif /* MEMP_OVERFLOW_CHECK || (LWIP_STATS && MEM_STATS) */
};
#endif /* MEM_USE_POOLS */

void  memp_init(void);

#if MEMP_OVERFLOW_CHECK
void *memp_malloc_fn(memp_t type, const char* file, const int line);
#define memp_malloc(t) memp_malloc_fn((t), __FILE__, __LINE__)
#else
void *memp_malloc(memp_t type);
#endif
void  memp_free(memp_t type, void *mem);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_MEMP_H */
