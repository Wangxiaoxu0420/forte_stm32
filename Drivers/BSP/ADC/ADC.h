#ifndef _ADC_DMA_H_
#define _ADC_DMA_H_
#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* ADC及引脚 定义 */

#define ADC_ADCX_CHY_GPIO_PORT              GPIOA
#define ADC_ADCX_CHY_GPIO_PIN               GPIO_PIN_5
#define ADC_ADCX_CHY_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)         /* PA口时钟使能 */

#define ADC_ADCX                            ADC1 
#define ADC_ADCX_CHY                        ADC_CHANNEL_5                                       /* 通道Y,  0 <= Y <= 17 */ 
#define ADC_ADCX_CHY_CLK_ENABLE()           do{ __HAL_RCC_ADC1_CLK_ENABLE(); }while(0)          /* ADC1 时钟使能 */

/******************************************************************************************/
/* ADC单通道/多通道 DMA采集 DMA数据流相关 定义
 * 注意: 这里我们的通道还是使用上面的定义.
 */
#define ADC_CH_NUM                          2                                               /* 需要转换的通道数目 */


#define ADC_ADCX_DMASx                      DMA2_Stream4
#define ADC_ADCX_DMASx_Chanel               DMA_CHANNEL_0                                   /* ADC1_DMA请求源 */
#define ADC_ADCX_DMASx_IRQn                 DMA2_Stream4_IRQn
#define ADC_ADCX_DMASx_IRQHandler           DMA2_Stream4_IRQHandler

#define ADC_ADCX_DMASx_IS_TC()              ( DMA2->HISR & (1 << 5) )                       /* 判断 DMA2_Stream4 传输完成标志, 这是一个假函数形式,
                                                                                             * 不能当函数使用, 只能用在if等语句里面
                                                                                             */
#define ADC_ADCX_DMASx_CLR_TC()             do{ DMA2->HIFCR |= 1 << 5; }while(0)            /* 清除 DMA2_Stream4 传输完成标志 */
void adc_init(void);    /* ADC初始化 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime);    /* ADC通道设置 */
uint32_t adc_get_result(uint32_t ch);       /* 获得某个通道值 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times);    /* 得到某个通道给定次数采样的平均值 */

void adc_dma_init(uint32_t mar);            /* ADC DMA采集初始化 */
void adc_dma_enable( uint16_t ndtr);        /* 使能一次ADC DMA采集传输 */

void adc_nch_dma_init(uint32_t tmr);        /* ADC多通道 DMA采集初始化 */
void adc_nch_dma_gpio_init(void);           /* ADC多通道 GPIO初始化 */
void adc_nch_dma_enable(uint16_t ndtr);     /* 使能一次ADC DMA多通道采集传输 */


void Adc1_Init(void);
uint16_t Get_Adc_Average(uint32_t ch,uint8_t times);

#endif

