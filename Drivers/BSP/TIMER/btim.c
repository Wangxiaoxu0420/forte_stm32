

#include "./BSP/TIMER/btim.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"


//extern uint32_t lwip_localtime;         /* lwip本地时间计数器,单位:ms */

TIM_HandleTypeDef g_tim3_handler;       /* 定时器参数句柄 */
TIM_HandleTypeDef g_tim6_handler;       /* 定时器参数句柄 */
//FreeRTOS时间统计所用的节拍计时器
volatile uint32_t FreeRTOSRunTimeTicks;	

/**
 * @brief       基本定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIM3_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim3_handler);  /* 定时器回调函数 */
}

void BTIM_TIM6_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim6_handler);  /* 定时器回调函数 */
}

/**
 * @brief       回调函数，定时器中断服务函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_tim3_handler))              /* 定时器3 */
    {
        FreeRTOSRunTimeTicks++;
    }
    else if(htim==(&g_tim6_handler))            /* 定时器6 */
    {
        //LED1_TOGGLE();
    }
}

/**
 * @brief       基本定时器TIMX定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为42M, 所以定时器时钟 = 84Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_tim3_int_init(uint16_t arr, uint16_t psc)
{
    g_tim3_handler.Instance = BTIM_TIM3_INT;                      /* 通用定时器X */
    g_tim3_handler.Init.Prescaler = psc;                          /* 设置预分频器  */
    g_tim3_handler.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 向上计数器 */
    g_tim3_handler.Init.Period = arr;                             /* 自动装载值 */
    g_tim3_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;   /* 时钟分频因子 */
    HAL_TIM_Base_Init(&g_tim3_handler);

    HAL_TIM_Base_Start_IT(&g_tim3_handler);                       /* 使能通用定时器x和及其更新中断：TIM_IT_UPDATE */
}

/**
 * @brief       基本定时器TIMX定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_tim6_int_init(uint16_t arr, uint16_t psc)
{
    g_tim6_handler.Instance = BTIM_TIM6_INT;                      /* 通用定时器X */
    g_tim6_handler.Init.Prescaler = psc;                          /* 设置预分频器  */
    g_tim6_handler.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 向上计数器 */
    g_tim6_handler.Init.Period = arr;                             /* 自动装载值 */
    g_tim6_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;   /* 时钟分频因子 */
    HAL_TIM_Base_Init(&g_tim6_handler);

    HAL_TIM_Base_Start_IT(&g_tim6_handler);                       /* 使能通用定时器x和及其更新中断：TIM_IT_UPDATE */
}

/**
 * @brief       定时器底册驱动，开启时钟，设置中断优先级
                此函数会被HAL_TIM_Base_Init()函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIM3_INT)
    {
        BTIM_TIM3_INT_CLK_ENABLE();                     /* 使能TIM时钟 */
        HAL_NVIC_SetPriority(BTIM_TIM3_INT_IRQn, 1, 3); /* 抢占1，子优先级3，组2 */
        HAL_NVIC_EnableIRQ(BTIM_TIM3_INT_IRQn);         /* 开启ITM3中断 */
    }
    if (htim->Instance == BTIM_TIM6_INT)
    {
        BTIM_TIM6_INT_CLK_ENABLE();                     /* 使能TIM时钟 */
        HAL_NVIC_SetPriority(BTIM_TIM6_INT_IRQn, 0, 3); /* 抢占1，子优先级3，组2 */
        HAL_NVIC_EnableIRQ(BTIM_TIM6_INT_IRQn);         /* 开启ITM3中断 */
    }
}

//初始化TIM3作为节拍计时器的时基
void ConfigureTimeForRunTimeStats(void)
{
	//TIM3初始化，定时器时钟为72M，分频系数72-1，所以TIM3频率为72M/72
	//1Mhz，重载值为50-1，那么TIM3周期为50us
	FreeRTOSRunTimeTicks = 0;
	btim_tim3_int_init(50-1,72-1);
}