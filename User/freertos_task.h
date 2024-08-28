#ifndef __FREERTOS_TASK_H
#define __FREERTOS_TASK_H
#include "sys_arch.h"
#define LORA_REC_TIMEOUT    3
#define SECOND_FOR_TICK 1000

#define ENABLE_SEND_PERIOD  1000

enum GATEWAY_EVENT{
    LORA_INIT_EVT=1,
    NETWORK_INIT_EVT=2,
    LORA_REC_EVT=4,
    LORA_EN_SEND_EVT=8,
    LORA_CMD_SEND_EVT=16,
    LORA_RSP_EVT=32,
    MQTT_CONN_EVT=64
};

/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1           /* 任务优先级 */
#define START_STK_SIZE          128         /* 任务堆栈大小 */
extern TaskHandle_t StartTask_Handler;             /* 任务句柄 */
void start_task(void *pvParameters);        /* 任务函数 */

/* LWIP_DEMO 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define T_HEART_PRIO     5          /* 任务优先级 */
#define T_HEART_STK_SIZE      512        /* 任务堆栈大小 */
extern TaskHandle_t T_Heart_Handler;             /* 任务句柄 */
void t_heart(void *pvParameters);    /* 任务函数 */

/* MONITOR_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define T_MONITOR_PRIO           6           /* 任务优先级 */
#define T_MONITOR_STK_SIZE            1024         /* 任务堆栈大小 */
extern TaskHandle_t T_Monitor_Handler;               /* 任务句柄 */
void t_monitor(void *pvParameters);          /* 任务函数 */

/* LORA_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define T_MBUS_CMD_PRIO           10           /* 任务优先级 */
#define T_MBUS_CMD_STK_SIZE            256         /* 任务堆栈大小 */
extern TaskHandle_t T_Mbus_Cmd_Handler;              /* 任务句柄 */
void t_mbus_cmd(void *pvParameters);      /* 任务函数 */

/* LORA_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define T_MBUS_POLL_PRIO           7           /* 任务优先级 */
#define T_MBUS_POLL_STK_SIZE            512         /* 任务堆栈大小 */
extern TaskHandle_t T_Mbus_Poll_Handler;              /* 任务句柄 */
void t_mbus_poll(void *pvParameters);      /* 任务函数 */


#define T_UPGRADE_PRIO       14 /* 发送数据线程优先级 */
#define T_UPGRADE_STK_SIZE            512         /* 任务堆栈大小 */
extern sys_thread_t t_upgrade_handler;
void t_upgrade(void *pvParameters);

#define T_LOAD_CONF_PRIO       13 /* 发送数据线程优先级 */
#define T_LOAD_CONF_STK_SIZE            512         /* 任务堆栈大小 */
extern sys_thread_t t_load_conf_handler;
void t_load_conf(void *pvParameters);


#define T_PUBLISH_PRIO        8 /* 发送数据线程优先级 */
#define T_PUBLISH_STK_SIZE            512         /* 任务堆栈大小 */
extern sys_thread_t t_publish_handler;
void t_publish(void *pvParameters);

#define T_UDP_PRIO       11 /* 发送数据线程优先级 */
#define T_UDP_STK_SIZE            512         /* 任务堆栈大小 */
extern sys_thread_t t_udp_handler;
void t_udp_thread(void *pvParameters);

#define T_TCP_PRIO       12 /* 发送数据线程优先级 */
#define T_TCP_STK_SIZE            512         /* 任务堆栈大小 */
extern sys_thread_t t_tcp_handler;
void t_tcp_thread(void *pvParameters);




void freertos_task(void);   /* 创建lwIP的任务函数 */

#endif
