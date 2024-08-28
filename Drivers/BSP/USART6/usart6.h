#ifndef __USART6_H
#define __USART6_H	 
#include "./SYSTEM/sys/sys.h"
   
/*******************************************************************************************************/

#define USART6_TX_GPIO_PORT              GPIOC
#define USART6_TX_GPIO_PIN               GPIO_PIN_6
#define USART6_TX_GPIO_AF                GPIO_AF8_USART6
#define USART6_TX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)  

#define USART6_RX_GPIO_PORT              GPIOC
#define USART6_RX_GPIO_PIN               GPIO_PIN_7
#define USART6_RX_GPIO_AF                GPIO_AF8_USART6
#define USART6_RX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)  

#define USART6_UX                        USART6
#define USART6_UX_IRQn                   USART6_IRQn
#define USART6_UX_IRQHandler             USART6_IRQHandler
#define USART6_UX_CLK_ENABLE()           do{ __HAL_RCC_USART6_CLK_ENABLE(); }while(0) 

/*******************************************************************************************************/

#define USART6_REC_LEN   200                     
#define USART6_EN_RX     1                       
#define U6RXBUFFERSIZE    1                       

extern UART_HandleTypeDef g_uart6_handle;     

extern uint8_t  g_usart6_rx_buf[USART6_REC_LEN];  
extern uint16_t g_usart6_rx_sta;              
extern uint8_t g_rx6_buffer[U6RXBUFFERSIZE];       

extern void u6_printf(char* fmt,...);
extern void u6_senddata(unsigned char* ptr,uint16_t len);

void usart6_init(uint32_t baudrate);      
void u6_sendbyte(unsigned char dat);

void usart6_reinit(uint32_t baudrate, uint8_t wordlength, uint8_t stopbits, uint8_t parity, uint8_t hwflowctl);

#endif


