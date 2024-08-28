#include <string.h>
#include <time.h>
#include "./SYSTEM/delay/delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "timers.h"
#include "freertos_task.h"


TaskHandle_t T_Monitor_Handler;               /* 任务句柄 */
TaskHandle_t StartTask_Handler;

/**
 * @breif       freertos_demo
 * @param       无
 * @retval      无
 */
void freertos_task(void)
{
    /* start_task任务 */
    xTaskCreate((TaskFunction_t )start_task,
                (const char *   )"start_task",
                (uint16_t       )START_STK_SIZE,
                (void *         )NULL,
                (UBaseType_t    )START_TASK_PRIO,
                (TaskHandle_t * )&StartTask_Handler);

    vTaskStartScheduler(); /* 开启任务调度 */
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    pvParameters = pvParameters;
    taskENTER_CRITICAL();           /* 进入临界区 */

    /* 创建lwIP任务 */
	#if 0	
	xTaskCreate((TaskFunction_t )lora_demo_run,
				(const char *   )"Lora_demo_task",
				(uint16_t       )LORA_STK_SIZE,
				(void*          )NULL,
				(UBaseType_t    )LORA_TASK_PRIO,
				(TaskHandle_t*  )&LORA_Task_Handler);
	#endif
	#if 0	
	xTaskCreate((TaskFunction_t )lora_send_task,
				(const char *   )"Lora_send_task",
				(uint16_t       )LORA_STK_SIZE/2,
				(void*          )NULL,
				(UBaseType_t    )LORA_TASK_PRIO-1,
				(TaskHandle_t*  )&LORA_SendTask_Handler);
	#endif
    #if 1
    /* MONITOR测试任务 */
    xTaskCreate((TaskFunction_t )t_monitor,
                (const char*    )"t_monitor",
                (uint16_t       )T_MONITOR_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )T_MONITOR_PRIO,
                (TaskHandle_t*  )&T_Monitor_Handler);
    #endif

    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
    
}


extern void forte_main(void);
void t_monitor(void *pvParameters)
{
    pvParameters = pvParameters;
    char *InfoBuffer;
    printf("forte_main start!! \n");
    forte_main();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("forte_main done \n");
    
	//开始任务调度，才认为采集器成功上电，再记录上电日志
  	//ERROR_REPORT(STR_REPORT_DEV_WARNING, "system restart!!!", 0);
    while (1)
    {
        //LED_BLINK();
		vTaskDelay(10);
    }
}
