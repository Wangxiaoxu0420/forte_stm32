#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./MALLOC/malloc.h"
#include "time.h"
#include "rtc.h"

extern void freertos_task(void);   /* 创建lwIP的任务函数 */


int main(void)
{
    /* 设置中断向量表偏移量为0X10000 */
    //sys_nvic_set_vector_table(FLASH_BASE, 0x10000);
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz */
    delay_init(168);                    /* 延时初始化 */

    rtc_init();                             /* 初始化RTC */
	rtc_set_wakeup(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0);  /* 配置WAKE UP中断,1秒钟中断一次 */
    usart_init(115200);                 /* 串口初始化为115200 */
    my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
    printf("\r\n\r\n");

    freertos_task();                    /* 创建lwIP的任务函数 */
}
