#ifndef __CC_H__
#define __CC_H__

#include "stdio.h"
 
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"

#define LWIP_NO_STDINT_H 1

typedef unsigned char u8_t;
typedef signed char s8_t;
typedef unsigned short u16_t;
typedef signed short s16_t;
typedef unsigned long u32_t;
typedef signed long s32_t;
typedef u32_t mem_ptr_t;
//typedef int sys_prot_t;

#define U16_F "hu"
#define S16_F "d"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"
#define SZT_F "uz"

/* ѡ��С��ģʽ */
#define BYTE_ORDER LITTLE_ENDIAN

/* define compiler specific symbols */
#if defined (__ICCARM__)
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_USE_INCLUDES

#elif defined (__CC_ARM)

#define PACK_STRUCT_BEGIN __packed
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#elif defined (__GNUC__)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
 
#elif defined (__TASKING__)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#endif


#define LWIP_PLATFORM_ASSERT(x) do {printf(x);}while(0)

 #ifdef __cplusplus
extern "C" {
#endif

extern u32_t sys_now(void);

#ifdef __cplusplus
}
#endif

#endif /* __CC_H__ */

