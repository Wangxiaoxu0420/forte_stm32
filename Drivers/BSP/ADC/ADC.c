
#include "./SYSTEM/delay/delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "./BSP/ADC/ADC.h"

ADC_HandleTypeDef ADC1_Handler;

void  Adc1_Init(void)
{    
	ADC1_Handler.Instance=ADC1;
	ADC1_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;//ADCCLK=PCLK2/4=80/4=21MHZ
	ADC1_Handler.Init.Resolution=ADC_RESOLUTION_12B; //12位
	ADC1_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT; //右对齐
	ADC1_Handler.Init.ScanConvMode=DISABLE; //不扫描
	ADC1_Handler.Init.EOCSelection=DISABLE; //关闭EOC
	ADC1_Handler.Init.ContinuousConvMode=DISABLE; //关闭连续转换
	ADC1_Handler.Init.NbrOfConversion=1;//1个转换规则序列
	ADC1_Handler.Init.DiscontinuousConvMode=DISABLE; //禁止不连续采样
	ADC1_Handler.Init.NbrOfDiscConversion=0; //不连续采样通道数为0
	ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START; //软件触发
	ADC1_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//没有外部触发
	ADC1_Handler.Init.DMAContinuousRequests=DISABLE; //关闭DMA
	
	HAL_ADC_Init(&ADC1_Handler); //初始化
}	

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{

	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_ADC1_CLK_ENABLE(); //使能 ADC1 时钟
	__HAL_RCC_GPIOA_CLK_ENABLE(); //开启 GPIOA 时钟
	
	GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_5; //PA4、PA5
	GPIO_Initure.Mode=GPIO_MODE_ANALOG; //模拟
	GPIO_Initure.Pull=GPIO_NOPULL; //不带上下拉
	
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

//获得 ADC 值
//ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
//返回值:转换结果
uint16_t Get_Adc(uint32_t ch)
{

	ADC_ChannelConfTypeDef ADC1_ChanConf;
	
	ADC1_ChanConf.Channel=ch; //通道
	ADC1_ChanConf.Rank=1; //第 1 个序列，序列 1
	ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES; //采样时间
	ADC1_ChanConf.Offset=0;
	HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf); //通道配置
	
	HAL_ADC_Start(&ADC1_Handler); //开启 ADC
	HAL_ADC_PollForConversion(&ADC1_Handler,10); //轮询转换

	return (uint16_t)HAL_ADC_GetValue(&ADC1_Handler);//返回最近一次 ADC1 规则组的转换结果
}

//获取指定通道的转换值，取 times 次,然后平均
//times:获取次数
//返回值:通道 ch 的 times 次转换结果平均值
uint16_t Get_Adc_Average(uint32_t ch,uint8_t times)
{
	uint32_t temp_val=0;
	uint8_t t;

	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
		//vTaskDelay(5);
	}

	return temp_val/times;
}


ADC_HandleTypeDef g_adc_handle;   /* ADC句柄 */

/* 单通道ADC采集 DMA读取 */
DMA_HandleTypeDef g_dma_adc_handle;     /* 与ADC关联的DMA句柄 */
uint8_t g_adc_dma_sta = 0;              /* DMA传输状态标志, 0,未完成; 1, 已完成 */

ADC_HandleTypeDef g_adc_nch_dma_handle;     /* 与DMA关联的ADC句柄 */
DMA_HandleTypeDef g_dma_nch_adc_handle;     /* 与ADC关联的DMA句柄 */


/**
 * @brief       多通道ADC的gpio初始化函数
 * @param       无
 * @note        此函数会被adc_nch_dma_init()调用
 * @note        PA0-ADC_CHANNEL_0、PA1-ADC_CHANNEL_1、PA2-ADC_CHANNEL_2
                PA3-ADC_CHANNEL_3、PA4-ADC_CHANNEL_4、PA5-ADC_CHANNEL_5
* @retval       无
 */
void adc_nch_dma_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_GPIOA_CLK_ENABLE();                    /* 开启GPIOA引脚时钟 */

    /* AD采集引脚模式设置,模拟输入 */
    //gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5; /* GPIOA0~5 */;
    gpio_init_struct.Pin = GPIO_PIN_4 | GPIO_PIN_5; /* GPIOA4&5 ，使用PA4检测电流，PA5检测电压*/;
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    gpio_init_struct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);
}

/**
 * @brief       使能一次ADC DMA传输
 * @param       ndtr: DMA传输的次数
 * @retval      无
 */
void adc_nch_dma_enable(uint16_t ndtr)
{
    __HAL_ADC_DISABLE(&g_adc_nch_dma_handle);       /* 先关闭ADC */
    
    __HAL_DMA_DISABLE(&g_dma_nch_adc_handle);       /* 关闭DMA传输 */
    g_dma_nch_adc_handle.Instance->NDTR = ndtr;     /* 重设DMA传输数据量 */
    __HAL_DMA_ENABLE(&g_dma_nch_adc_handle);        /* 开启DMA传输 */
    
    __HAL_ADC_ENABLE(&g_adc_nch_dma_handle);        /* 重新启动ADC */
    ADC_ADCX->CR2 |= 1 << 30;                       /* 启动规则转换通道 */
}


/**
 * @brief       ADC初始化函数
 *   @note      本函数支持ADC1/ADC2任意通道, 但是不支持ADC3
 *              我们使用12位精度, ADC采样时钟=21M, 转换时间为: 采样周期 + 12个ADC周期
 *              设置最大采样周期: 480, 则转换时间 = 492 个ADC周期 = 23.42us
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
    g_adc_handle.Instance = ADC_ADCX;
    g_adc_handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;        /* 4分频，ADCCLK = PCLK2/4 = 84/4 = 21Mhz */
    g_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;                      /* 12位模式 */
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                      /* 右对齐 */
    g_adc_handle.Init.ScanConvMode = DISABLE;                               /* 非扫描模式 */
    g_adc_handle.Init.ContinuousConvMode = ENABLE;                          /* 开启连续转换 */
    g_adc_handle.Init.NbrOfConversion = 1;                                  /* 1个转换在规则序列中 也就是只转换规则序列1 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;                      /* 禁止不连续采样模式 */
    g_adc_handle.Init.NbrOfDiscConversion = 0;                              /* 不连续采样通道数为0 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* 软件触发 */
    g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* 使用软件触发 */
    g_adc_handle.Init.DMAContinuousRequests = ENABLE;                       /* 开启DMA请求 */
    HAL_ADC_Init(&g_adc_handle);                                            /* 初始化 */
}
/**
 * @brief       设置ADC通道采样时间
 * @param       adcx : adc句柄指针,ADC_HandleTypeDef
 * @param       ch   : 通道号, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       stime: 采样时间  0~7, 对应关系为:
 *   @arg       ADC_SAMPLETIME_3CYCLES,  3个ADC时钟周期        ADC_SAMPLETIME_15CYCLES, 15个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_28CYCLES, 28个ADC时钟周期       ADC_SAMPLETIME_56CYCLES, 56个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_84CYCLES, 84个ADC时钟周期       ADC_SAMPLETIME_112CYCLES,112个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_144CYCLES,144个ADC时钟周期      ADC_SAMPLETIME_480CYCLES,480个ADC时钟周期
 * @param       rank: 多通道采集时需要设置的采集编号,
                假设你定义channel1的rank=1，channel2 的rank=2，
                那么对应你在DMA缓存空间的变量数组AdcDMA[0] 就i是channel1的转换结果，AdcDMA[1]就是通道2的转换结果。 
                单通道DMA设置为 ADC_REGULAR_RANK_1
 *   @arg       编号1~16：ADC_REGULAR_RANK_1~ADC_REGULAR_RANK_16
 * @retval      无
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime)
{
    /* 配置对应ADC通道 */
    ADC_ChannelConfTypeDef adc_channel;
    adc_channel.Channel = ch;               /* 设置ADCX对通道ch */
    adc_channel.Rank = rank;                /* 设置采样序列 */
    adc_channel.SamplingTime = stime;       /* 设置采样时间 */
    HAL_ADC_ConfigChannel(adc_handle, &adc_channel); /* 初始化ADC通道 */
}

/**
 * @brief       ADC N通道(6通道) DMA读取 初始化函数
 *   @note      本函数还是使用adc_init对ADC进行大部分配置, 有差异的地方再单独配置
 *              另外, 由于本函数用到了6个通道, 宏定义会比较多内容, 因此, 本函数就不采用宏定义的方式来修改通道了,
 *              直接在本函数里面修改, 这里我们默认使用PA0~PA5这6个通道.
 *
 *              注意: 本函数还是使用 ADC_ADCX(默认=ADC1) 和 ADC_ADCX_DMASx( DMA2_Stream4 ) 及其相关定义
 *              不要乱修改adc.h里面的这两部分内容, 必须在理解原理的基础上进行修改, 否则可能导致无法正常使用.
 *
 * @param       mar         : 存储器地址
 * @retval      无
 */
void adc_nch_dma_init(uint32_t mar)
{
    ADC_ADCX_CHY_CLK_ENABLE();                          /* 使能ADCx时钟 */
    
    if ((uint32_t)ADC_ADCX_DMASx > (uint32_t)DMA2)      /* 大于DMA1_Stream7, 则为DMA2 */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                    /* DMA2时钟使能 */
    }
    else
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                    /* DMA1时钟使能 */
    }

    /* DMA配置 */
    g_dma_nch_adc_handle.Instance = ADC_ADCX_DMASx;                             /* 设置DMA数据流 */
    g_dma_nch_adc_handle.Init.Channel = DMA_CHANNEL_0;                          /* 设置DMA通道 */
    g_dma_nch_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* DIR = 1 , 外设到存储器模式 */
    g_dma_nch_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* 外设非增量模式 */
    g_dma_nch_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* 存储器增量模式 */
    g_dma_nch_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* 外设数据长度:16位 */
    g_dma_nch_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* 存储器数据长度:16位 */
    g_dma_nch_adc_handle.Init.Mode = DMA_NORMAL;                                /* 外设流控模式 */
    g_dma_nch_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* 中等优先级 */
    HAL_DMA_Init(&g_dma_nch_adc_handle);                                        /* 初始化DMA */
    HAL_DMA_Start(&g_dma_nch_adc_handle, (uint32_t)&ADC_ADCX->DR, mar, 0);      /* 配置DMA传输参数 */

    g_adc_nch_dma_handle.DMA_Handle = &g_dma_nch_adc_handle;    /* 设置ADC对应的DMA */

    g_adc_nch_dma_handle.Instance = ADC_ADCX;
    g_adc_nch_dma_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;            /* 4分频，ADCCLK = PCLK2/4 = 84/4 = 21Mhz */
    g_adc_nch_dma_handle.Init.Resolution = ADC_RESOLUTION_12B;                      /* 12位模式 */
    g_adc_nch_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                      /* 右对齐 */
    g_adc_nch_dma_handle.Init.ScanConvMode = ENABLE;                                /* 扫描模式 */
    g_adc_nch_dma_handle.Init.ContinuousConvMode = ENABLE;                          /* 连续转换模式，转换完成之后接着继续转换 */
    g_adc_nch_dma_handle.Init.DiscontinuousConvMode = DISABLE;                      /* 禁止不连续采样模式 */
    g_adc_nch_dma_handle.Init.NbrOfConversion = ADC_CH_NUM;                         /* 使用转换通道数，需根据实际转换通道去设置 */
    g_adc_nch_dma_handle.Init.NbrOfDiscConversion = 0;                              /* 不连续采样通道数为0 */
    g_adc_nch_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* 软件触发 */
    g_adc_nch_dma_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* 使用软件触发, 此位忽略 */
    g_adc_nch_dma_handle.Init.DMAContinuousRequests = ENABLE;                       /* 开启DMA连续转换 */
    HAL_ADC_Init(&g_adc_nch_dma_handle);                                            /* 初始化ADC */

    adc_nch_dma_gpio_init();    /* GPIO 初始化 */

    //adc_channel_set(&g_adc_nch_dma_handle, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_480CYCLES);  /* 设置采样规则序列1~6 */
    //adc_channel_set(&g_adc_nch_dma_handle, ADC_CHANNEL_1, 2, ADC_SAMPLETIME_480CYCLES);
    //adc_channel_set(&g_adc_nch_dma_handle, ADC_CHANNEL_2, 3, ADC_SAMPLETIME_480CYCLES);
    //adc_channel_set(&g_adc_nch_dma_handle, ADC_CHANNEL_3, 4, ADC_SAMPLETIME_480CYCLES);
    adc_channel_set(&g_adc_nch_dma_handle, ADC_CHANNEL_4, 1, ADC_SAMPLETIME_480CYCLES);
    adc_channel_set(&g_adc_nch_dma_handle, ADC_CHANNEL_5, 2, ADC_SAMPLETIME_480CYCLES);

    HAL_NVIC_SetPriority(ADC_ADCX_DMASx_IRQn, 3, 3);    /* 设置DMA中断优先级为3，子优先级为3 */
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMASx_IRQn);            /* 使能DMA中断 */
    HAL_ADC_Start_DMA(&g_adc_nch_dma_handle, &mar, sizeof(uint16_t));   /* 开始DMA数据传输 */
    __HAL_DMA_ENABLE_IT(&g_dma_nch_adc_handle, DMA_IT_TC);              /* TCIE=1, 使能传输完成中断 */
}

/**
 * @brief       ADC DMA采集中断服务函数
 * @param       无
 * @retval      无
 */
void ADC_ADCX_DMASx_IRQHandler(void)
{
    if (ADC_ADCX_DMASx_IS_TC())
    {
        g_adc_dma_sta = 1;          /* 标记DMA传输完成 */
        ADC_ADCX_DMASx_CLR_TC();    /* 清除DMA2 数据流4 传输完成中断 */
    }
}
