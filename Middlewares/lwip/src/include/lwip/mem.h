
#ifndef LWIP_HDR_MEM_H
#define LWIP_HDR_MEM_H

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

#if MEM_LIBC_MALLOC

#include "lwip/arch.h"

typedef size_t mem_size_t;
#define MEM_SIZE_F SZT_F

#elif MEM_USE_POOLS

typedef u16_t mem_size_t;
#define MEM_SIZE_F U16_F

#else

/* MEM_SIZE would have to be aligned, but using 64000 here instead of
 * 65535 leaves some room for alignment...
 */
#if MEM_SIZE > 64000L
typedef u32_t mem_size_t;
#define MEM_SIZE_F U32_F
#else
typedef u16_t mem_size_t;
#define MEM_SIZE_F U16_F
#endif /* MEM_SIZE > 64000 */
#endif

void  mem_init(void);
void *mem_trim(void *mem, mem_size_t size);
void *mem_malloc(mem_size_t size);
void *mem_calloc(mem_size_t count, mem_size_t size);
void  mem_free(void *mem);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_MEM_H */
