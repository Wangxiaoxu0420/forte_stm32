/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "shell.h"
#include "stm32f4xx_hal.h"
#include "./SYSTEM/usart/usart.h"


Shell shell;
char shellBuffer[512];
static SemaphoreHandle_t shellMutex;

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{

    HAL_UART_Transmit(&g_uart1_handle, (uint8_t *)data, len, 0x1FF);
    return len;
}


/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际读取到
 */
short userShellRead(char *data, unsigned short len)
{
    if (g_usart_rx_sta & 0x3fff)        /* 串口接收完成？ */
    {
        *data = g_usart_rx_buf[0];
        g_usart_rx_sta = 0;//清楚接收
        return 1;
    }
    else
    {
        return 0;
    }

    
}

/**
 * @brief 用户shell上锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellLock(Shell *shell)
{
    xSemaphoreTakeRecursive(shellMutex, portMAX_DELAY);
    return 0;
}

/**
 * @brief 用户shell解锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellUnlock(Shell *shell)
{
    xSemaphoreGiveRecursive(shellMutex);
    return 0;
}

/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
    shellMutex = xSemaphoreCreateMutex();

    shell.write = userShellWrite;
    shell.read = userShellRead;
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;
    shellInit(&shell, shellBuffer, 512);

    /** shell 文件系统初始化 **/
    userShellFsInit();
    /** shell 文件系统初始化结束 **/

    if (xTaskCreate(shellTask, "shell", 1024, &shell, 1, NULL) != pdPASS)
    {
        //logError("shell task creat failed");
    }
}

void shellTest(int a, int b, int c)
{
	shellPrint(&shell, "This is test\r\n");
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
					test, shellTest, This is test);





