
 
#ifndef _LWIP_DEMO_H
#define _LWIP_DEMO_H
#include "./SYSTEM/sys/sys.h"

/* 用户需要根�?设�?�信�?完善以下宏定义中的三元组内�?? */
#define PRODUCT_KEY         "a1S0GvSCHqA"         /* �?一标识�?11位长度的英文数字随机组合 */
#define DEVICE_NAME         "WATERLOA"               /* 用户注册设�?�时生成的�?��?�唯一编号，支持系统自动生成，也可�?持用户添加自定义编号，产品维度内�?一  */
#define DEVICE_SECRET       NULL                  /* 设�?�密钥，与DeviceName成�?�出现，�?用于一机一密的认证方�??  */

/* 以下参数的宏定义固定，不需要修改，�?�?改上方的参数即可 */
#define HOST_NAME           NULL                         /* 域名 */
#define HOST_PORT           1883                         /* 固定1883 */
#define CONTENT             NULL                         /* 计算登录密码�? */
//#define CLIENT_ID           DEVICE_NAME"&"PRODUCT_KEY    /* 客户端ID */
#define CLIENT_ID           &COLLECTORID[8]    /* 客户端ID */
#define USER_NAME           NULL                         /* 客户�?用户�? */
//#define DEVICE_PUBLISH      "local/waterlog/status/"        /* 发布 */
//#define DEVICE_SUBSCRIBE    "local/waterlog/control/"       /* 订阅 */

/* start: wangxiaoxu 20230605 lable_001 */
/* reason: add PUBLISH and SUBSCRIBE directory */
#define DEVICE_PUBLISH_WORKSTATUS       "local/sbdm/device/workstatus"
#define DEVICE_PUBLISH_CONFIG           "local/sbdm/device/config"
#define DEVICE_PUBLISH_ALARM            "local/sbdm/device/alarm"
#define DEVICE_SUBSCRIBE_CONTROL        "local/sbdm/device_id/control"
#define DEVICE_SUBSCRIBE_CONFIG         "local/sbdm/device_id/config"
#define DEVICE_PUBLISH           "local/status/%s"
#define DEVICE_SUBSCRIBE         "local/control/%s"

/* end: wangxiaoxu 20230605 lable_001 */


void num_to_str(uint32_t number, uint8_t *string);

void lwip_app(void);


#endif /* _CLIENT_H */
