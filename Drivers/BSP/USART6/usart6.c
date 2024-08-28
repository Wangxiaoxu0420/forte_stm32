#include "./SYSTEM/sys/sys.h"
#include "usart6.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 
#include "string.h"	 

uint8_t USART6_TX_BUF[100];

void u6_senddata(unsigned char* ptr,uint16_t len)
{
	uint16_t i;
	
	for(i=0;i<len;i++)//循环发送数据
	{
		while ((USART6->SR & 0X40) == 0);  //等待上次传输完成 
		USART6->DR = (uint8_t)ptr[i]; 	 //发送数据到串口
	}
}

void u6_sendbyte(unsigned char dat)
{
	while ((USART6->SR & 0X40) == 0);  //等待上次传输完成 
	USART6->DR = dat; 	 //发送数据到串口
}

//串口printf 函数
//确保一次发送数据不超过USART6_MAX_SEND_LEN字节
void u6_printf(char* fmt,...)  
{  
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART6_TX_BUF,fmt,ap);
	va_end(ap);

	u6_senddata(USART6_TX_BUF,strlen((const char*)USART6_TX_BUF));
}
    
#if USART6_EN_RX                                     /* 如果使能了接收 */

/* 接收缓冲, 最大USART6_REC_LEN个字节. */
uint8_t g_usart6_rx_buf[USART6_REC_LEN];

/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
uint16_t g_usart6_rx_sta = 0;

uint8_t g_rx6_buffer[U6RXBUFFERSIZE];                  /* HAL库使用的串口接收缓冲 */

UART_HandleTypeDef g_uart6_handle;                  /* UART句柄 */


/**
 * @brief       串口X初始化函数
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @note        注意: 必须设置正确的时钟源, 否则串口波特率就会设置异常.
 *              这里的USART的时钟源在sys_stm32_clock_init()函数中已经设置过了.
 * @retval      无
 */
void usart6_init(uint32_t baudrate)
{
    g_uart6_handle.Instance = USART6_UX;                         /* USART1 */
    g_uart6_handle.Init.BaudRate = baudrate;                    /* 波特率 */
    g_uart6_handle.Init.WordLength = UART_WORDLENGTH_8B;        /* 字长为8位数据格式 */
    g_uart6_handle.Init.StopBits = UART_STOPBITS_1;             /* 一个停止位 */
    g_uart6_handle.Init.Parity = UART_PARITY_NONE;              /* 无奇偶校验位 */
    g_uart6_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;        /* 无硬件流控 */
    g_uart6_handle.Init.Mode = UART_MODE_TX_RX;                 /* 收发模式 */
    HAL_UART_Init(&g_uart6_handle);                             /* HAL_UART_Init()会使能UART1 */
    
    /* 该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量 */
    HAL_UART_Receive_IT(&g_uart6_handle, (uint8_t *)g_rx6_buffer, U6RXBUFFERSIZE);
}

void usart6_reinit(uint32_t baudrate, uint8_t wordlength, uint8_t stopbits, uint8_t parity, uint8_t hwflowctl)
{
    uint8_t dif = 0;
    uint32_t stopbits_buf[3] = {0, UART_STOPBITS_1, UART_STOPBITS_2};
    uint32_t parity_buf[3] = {UART_PARITY_NONE, UART_PARITY_EVEN, UART_PARITY_ODD};
    uint32_t hwflowctl_buf[4] = {UART_HWCONTROL_NONE, UART_HWCONTROL_RTS, UART_HWCONTROL_CTS, UART_HWCONTROL_RTS_CTS};

    if(g_uart6_handle.Init.BaudRate != baudrate)                    dif = 1;                    /* 波特率 */
    if(wordlength == 9)
    {
        if(g_uart6_handle.Init.WordLength != UART_WORDLENGTH_9B)    dif = 1;        /* 字长为8位数据格式 */
    }
    else{
        if(g_uart6_handle.Init.WordLength != UART_WORDLENGTH_8B)    dif = 1;        /* 字长为8位数据格式 */
    }
    if(g_uart6_handle.Init.StopBits  != stopbits_buf[stopbits])     dif = 1;             /* 一个停止位 */
    if(g_uart6_handle.Init.Parity    != parity_buf[parity])         dif = 1;             /* 无奇偶校验位 */
    if(g_uart6_handle.Init.HwFlowCtl != hwflowctl_buf[hwflowctl])   dif = 1;       /* 无硬件流控 */

    //不需要配置
    if(dif == 0) return;
    
    //入参校验，非法值使用默认串口数据
    if( (baudrate < 300) 
     || (baudrate > 115200)
     || (baudrate % 300)
    )
    {
        baudrate = 9600;
    }
    if(stopbits > 2) stopbits = 2;
    if(parity > 2) parity = 2;
    if(hwflowctl > 3) hwflowctl = 3;

    g_uart6_handle.Instance = USART6_UX;                         /* USART1 */
    g_uart6_handle.Init.BaudRate = baudrate;                    /* 波特率 */
    if(wordlength == 9)
    {
        g_uart6_handle.Init.WordLength = UART_WORDLENGTH_9B;        /* 字长为8位数据格式 */
    }
    else{
        g_uart6_handle.Init.WordLength = UART_WORDLENGTH_8B;        /* 字长为8位数据格式 */
    }
    g_uart6_handle.Init.StopBits = stopbits_buf[stopbits];             /* 一个停止位 */
    g_uart6_handle.Init.Parity = parity_buf[parity];              /* 无奇偶校验位 */

    if(g_uart6_handle.Init.Parity != UART_PARITY_NONE)
    {
        g_uart6_handle.Init.WordLength = UART_WORDLENGTH_9B;        /* 字长为8位数据格式 */
    }

    g_uart6_handle.Init.HwFlowCtl = hwflowctl_buf[hwflowctl];        /* 无硬件流控 */
    g_uart6_handle.Init.Mode = UART_MODE_TX_RX;                 /* 收发模式 */
    HAL_UART_Init(&g_uart6_handle);                             /* HAL_UART_Init()会使能UART1 */
    
    /* 该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量 */
    HAL_UART_Receive_IT(&g_uart6_handle, (uint8_t *)g_rx6_buffer, U6RXBUFFERSIZE);
    vTaskDelay(500);//等待串口总线稳定
}

/**
 * @brief       UART底层初始化函数
 * @param       huart: UART句柄类型指针
 * @note        此函数会被HAL_UART_Init()调用
 *              完成时钟使能，引脚配置，中断配置
 * @retval      无
 */
void HAL_UART6_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;
    if(huart->Instance == USART6_UX)                             /* 如果是串口6，进行串口6 MSP初始化 */
    {
        USART6_UX_CLK_ENABLE();                                  /* USART1 时钟使能 */
        USART6_TX_GPIO_CLK_ENABLE();                             /* 发送引脚时钟使能 */
        USART6_RX_GPIO_CLK_ENABLE();                             /* 接收引脚时钟使能 */

        gpio_init_struct.Pin = USART6_TX_GPIO_PIN;               /* TX引脚 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
        gpio_init_struct.Alternate = USART6_TX_GPIO_AF;          /* 复用为USART1 */
        HAL_GPIO_Init(USART6_TX_GPIO_PORT, &gpio_init_struct);   /* 初始化发送引脚 */

        gpio_init_struct.Pin = USART6_RX_GPIO_PIN;               /* RX引脚 */
        gpio_init_struct.Alternate = USART6_RX_GPIO_AF;          /* 复用为USART1 */
        HAL_GPIO_Init(USART6_RX_GPIO_PORT, &gpio_init_struct);   /* 初始化接收引脚 */

#if USART6_EN_RX
        HAL_NVIC_EnableIRQ(USART6_UX_IRQn);                      /* 使能USART2中断通道 */
        HAL_NVIC_SetPriority(USART6_UX_IRQn, 2, 2);              /* 抢占优先级2，子优先级2 */
#endif
    }
}

/**
 * @brief       Rx传输回调函数
 * @param       huart: UART句柄类型指针
 * @retval      无
 */
void HAL_UART6_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART6_UX)             /* 如果是串口1 */
    {
        if((g_usart6_rx_sta & 0x8000) == 0)      /* 接收未完成 */
        {
            /* start: wangxiaoxu 20230607 lable_002 */
            /* reason: modify modbus protocol. */
            #if 0
            if(g_usart6_rx_sta & 0x4000)         /* 接收到了0x0d */
            {
                if(g_rx6_buffer[0] != 0x0a) 
                {
                    g_usart6_rx_sta = 0;         /* 接收错误,重新开始 */
                }
                else 
                {
                    g_usart6_rx_sta |= 0x8000;   /* 接收完成了 */
                }
            }
            else                                /* 还没收到0X0D */
            {
                if(g_rx6_buffer[0] == 0x0d)
                {
                    g_usart6_rx_sta |= 0x4000;
                }
                else
                {
            #endif
                    g_usart6_rx_buf[g_usart6_rx_sta & 0X3FFF] = g_rx6_buffer[0];
                    g_usart6_rx_sta++;
                    //终端设备协议头尾，包括地址1字节，功能码1字节，数据长度1字节，CRC校验2字节，故长度固定加5                    
                    if ( (g_usart6_rx_sta >= 2) && (g_usart6_rx_sta == (g_usart6_rx_buf[2]+5) ))
                    {
                        g_usart6_rx_sta |= 0x8000;     /* 终端设备回传数据会附带字节数，接收到指定数量的字符后，判断为接受完成，不再接受后续字符 */
                        return;
                    }
                    //modbus RTU中05/06/15/16功能码，固定8字节长度，只判断一次，判断功能码是否为05/06/15/16
                    if ( (g_usart6_rx_sta == 8) && 
                         ( (g_usart6_rx_buf[1] == 5) 
                        || (g_usart6_rx_buf[1] == 6) 
                        || (g_usart6_rx_buf[1] == 15) 
                        || (g_usart6_rx_buf[1] == 16)) )
                    {
                        g_usart6_rx_sta |= 0x8000;     /* 终端设备回传数据会附带字节数，接收到指定数量的字符后，判断为接受完成，不再接受后续字符 */
                        return;
                    }
                    
                    if ( (g_usart6_rx_sta == 5) && (g_usart6_rx_buf[1] & 0x80)) //如果返回错误，最高位置1，返回字符为5个字节
                    {
                        g_usart6_rx_sta |= 0x8000;     /* 终端设备回传数据会附带字节数，接收到指定数量的字符后，判断为接受完成，不再接受后续字符 */
                        return;
                    }
                    if(g_usart6_rx_sta > (USART6_REC_LEN - 1))
                    {
                        g_usart6_rx_sta = 0;     /* 接收数据错误,重新开始接收 */
                    }
                #if 0
                }
            }
                #endif
        }
    }
}

/**
 * @brief       串口1中断服务函数
 * @param       无
 * @retval      无
 */
void USART6_UX_IRQHandler(void)
{ 
    uint32_t timeout = 0;
    uint32_t maxDelay = 0x1FFFF;
    

    HAL_UART_IRQHandler(&g_uart6_handle);       /* 调用HAL库中断处理公用函数 */

    timeout = 0;
    while (HAL_UART_GetState(&g_uart6_handle) != HAL_UART_STATE_READY) /* 等待就绪 */
    {
        timeout++;                              /* 超时处理 */
        if(timeout > maxDelay)
        {
            break;
        }
    }
     
    timeout=0;
    
    /* 一次处理完成之后，重新开启中断并设置RxXferCount为1 */
    while (HAL_UART_Receive_IT(&g_uart6_handle, (uint8_t *)g_rx6_buffer, U6RXBUFFERSIZE) != HAL_OK)
    {
        timeout++;                              /* 超时处理 */
        if (timeout > maxDelay)
        {
            break;
        }
    }

}

#endif


 

 




