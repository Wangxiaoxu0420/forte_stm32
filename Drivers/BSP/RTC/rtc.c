#include "./SYSTEM/sys/sys.h"
#include "time.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/LED/led.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"


RTC_HandleTypeDef g_rtc_handle;

//rtc 写入后备区SRAM

void rtc_write_bkr(uint32_t bkrx,uint32_t data)
{
	HAL_PWR_EnableBkUpAccess();
	HAL_RTCEx_BKUPWrite(&g_rtc_handle,bkrx,data);
}

//RTC 读取后备区域SRAM
uint32_t rtc_read_bkr(uint32_t bkrx)
{
	uint32_t temp = 0;
	
	temp = HAL_RTCEx_BKUPRead(&g_rtc_handle,bkrx);
	return temp;
}

HAL_StatusTypeDef rtc_set_time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm)
{
	RTC_TimeTypeDef rtc_time_handle;
	
	rtc_time_handle.Hours = hour;
	rtc_time_handle.Minutes = min;
	rtc_time_handle.Seconds = sec;
	rtc_time_handle.TimeFormat = ampm;
	rtc_time_handle.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	rtc_time_handle.StoreOperation = RTC_STOREOPERATION_RESET;
	return HAL_RTC_SetTime(&g_rtc_handle, &rtc_time_handle,RTC_FORMAT_BIN);
}


/**
 * @brief       RTC日期设置
 * @param       year,month,date : 年(0~99),月(1~12),日(0~31)
 * @param       week            : 星期(1~7,0,非法!)
 * @retval      0,成功
 *              其他,异常状态
 */
HAL_StatusTypeDef rtc_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{
    RTC_DateTypeDef rtc_date_handle;

    rtc_date_handle.Date = date;
    rtc_date_handle.Month = month;
    rtc_date_handle.WeekDay = week;
    rtc_date_handle.Year = year;
    return HAL_RTC_SetDate(&g_rtc_handle, &rtc_date_handle, RTC_FORMAT_BIN);
}

/**
 * @brief       获取RTC时间
 * @param       *hour,*min,*sec : 小时,分钟,秒钟
 * @param       *ampm           : AM/PM,0=AM/24H,1=PM/12H.
 * @retval      无
 */
void rtc_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *ampm)
{
    
    RTC_TimeTypeDef rtc_time_handle;

    HAL_RTC_GetTime(&g_rtc_handle, &rtc_time_handle, RTC_FORMAT_BIN);

    *hour = rtc_time_handle.Hours;
    *min = rtc_time_handle.Minutes;
    *sec = rtc_time_handle.Seconds;
    *ampm = rtc_time_handle.TimeFormat;
}

/**
 * @brief       获取RTC日期
 * @param       *year,*mon,*date: 年,月,日
 * @param       *week           : 星期
 * @retval      无
 */
void rtc_get_date(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *week)
{
    RTC_DateTypeDef rtc_date_handle;

    HAL_RTC_GetDate(&g_rtc_handle, &rtc_date_handle, RTC_FORMAT_BIN);

    *year = rtc_date_handle.Year;
    *month = rtc_date_handle.Month;
    *date = rtc_date_handle.Date;
    *week = rtc_date_handle.WeekDay;
}

/* 月修正数据表 */
uint8_t const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};

/**
 * @breif       获得现在是星期几, 输入公历日期得到星期(只允许1901-2099年)
 * @param       year,month,day : 公历年月日
 * @retval      星期号(1~7,代表周1~周日)
 */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp;
    uint8_t yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;

    /*  如果为21世纪,年份数加100 */
    if (yearH > 19)
    {
        yearL += 100;
    }

    /*  所过闰年数只算1900年之后的 */
    temp = yearL + yearL / 4;
    temp = temp % 7;
    temp = temp + day + table_week[month - 1];

    if (yearL % 4 == 0 && month < 3)
    {
        temp--;
    }

    temp %= 7;

    if (temp == 0)
    {
        temp = 7;
    }

    return temp;
}

/**
 * @brief       RTC初始化
 *   @note
 *              默认尝试使用LSE,当LSE启动失败后,切换为LSI.
 *              通过BKP寄存器0的值,可以判断RTC使用的是LSE/LSI:
 *              当BKP0==0x5050时,使用的是LSE
 *              当BKP0==0x5051时,
 *              注意:切换LSI/LSE将导致时间/日期丢失,切换后需重新设置.
 *
 * @param       无
 * @retval      0,成功
 *              1,进入初始化模式失败
 */
uint8_t rtc_init(void)
{
    uint16_t bkpflag = 0;
    
    __HAL_RCC_RTC_ENABLE();     /* RTC使能 */
    HAL_PWR_EnableBkUpAccess(); /* 取消备份区域写保护 */
    __HAL_RCC_PWR_CLK_ENABLE(); /* 使能电源时钟PWR */
    
    g_rtc_handle.Instance = RTC;
    g_rtc_handle.Init.HourFormat = RTC_HOURFORMAT_24;   /* RTC设置为24小时格式 */
    g_rtc_handle.Init.AsynchPrediv = 0x7F;              /* RTC异步分频系数(1~0x7F) */
    g_rtc_handle.Init.SynchPrediv = 0xFF;               /* RTC同步分频系数(0~0x7FFF) */
    g_rtc_handle.Init.OutPut = RTC_OUTPUT_DISABLE;
    g_rtc_handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    g_rtc_handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    /* 检查是不是第一次配置时钟 */
    bkpflag = rtc_read_bkr(0);                /* 读取BKP0的值 */

    if (HAL_RTC_Init(&g_rtc_handle) != HAL_OK)
    {
        return 1;
    }

    if ((bkpflag != 0x5050) && (bkpflag != 0x5051))     /* 之前未初始化过, 重新配置 */
    {
        rtc_set_time(6, 59, 56, RTC_HOURFORMAT12_AM);   /* 设置时间, 根据实际时间修改 */
        rtc_set_date(20, 4, 22, 3);                     /* 设置日期 */
    }
    return 0;
}


void GetTimeInfo(struct tm *info)
{
	uint8_t hour, min, sec, ampm;
    uint8_t year, month, date, week;
	rtc_get_time(&hour, &min, &sec, &ampm);
    rtc_get_date(&year, &month, &date, &week);

	//printf("Current year:%d,month:%d,date:%d,hour:%d,min:%d,sec:%d\r\n",year,month,date,hour,min,sec);
	info->tm_year = year+100;
	info->tm_mon = month - 1;
	info->tm_mday = date;
	info->tm_hour = hour;
	info->tm_min = min;
	info->tm_sec = sec;

	//printf("Make date:%d-%d-%d %d:%d:%d\r\n",info->tm_year,info->tm_mon,info->tm_mday,
	//                                         info->tm_hour,info->tm_min,info->tm_sec);
}

void GetTimeStr(char *pcTime)
{
    struct tm timeinfo;
    GetTimeInfo(&timeinfo);
    strftime(pcTime,50,"[%Y-%m-%d %H:%M:%S]",&timeinfo);
}
uint32_t GetTimeStamp32(void)
{
    struct tm timeinfo;
    uint32_t Timestamp;
    GetTimeInfo(&timeinfo);
    Timestamp = mktime(&timeinfo);
    //printf("time stamp is 0x%x!!\r\n", Timestamp);

    return Timestamp;
}
uint64_t GetTimeStamp(void)
{
    struct tm timeinfo;
    uint64_t Timestamp;
    GetTimeInfo(&timeinfo);
    Timestamp = mktime(&timeinfo);
    //printf("time stamp is 0x%x!!\r\n", Timestamp);

    return Timestamp*1000 + (HAL_GetTick()%1000);
}
//设备需要有更新时间戳的功能，XTBOX下发消息会附带时间戳，如果跟本地时间相差5s以上，则更新本地时间戳
//更新当前时间戳，如果与XTBOX下发时间误差超过5s，则更新RTC时钟
void _UpdateTime(uint32_t Timestamp)
{
    struct tm timeinfo_local;
    struct tm *timeinfo;
    uint32_t Timestamp_local;
    struct tm rtc;

    GetTimeInfo(&timeinfo_local);
    Timestamp_local = mktime(&timeinfo_local);
    
    if (
        ( ( Timestamp_local > Timestamp ) && ( (Timestamp_local - Timestamp) >= 5) )
        || ( ( Timestamp_local < Timestamp ) && ( (Timestamp - Timestamp_local) >= 5) )
    )
    {
        //如果差值大于5s，则更新RTC时钟
        timeinfo = localtime(&Timestamp);
        rtc.tm_year = timeinfo->tm_year - 100;
        rtc.tm_mon = timeinfo->tm_mon + 1;
        rtc.tm_mday = timeinfo->tm_mday;
        rtc.tm_wday = timeinfo->tm_wday;
        rtc.tm_hour = timeinfo->tm_hour;
        rtc.tm_min = timeinfo->tm_min;
        rtc.tm_sec = timeinfo->tm_sec;
        
        rtc_set_time(rtc.tm_hour,rtc.tm_min,rtc.tm_sec,0);
        rtc_set_date(rtc.tm_year,rtc.tm_mon,rtc.tm_mday,rtc.tm_wday);
    }
    
}


/**
 * @brief       RTC底层驱动, 时钟配置
 * @param       hrtc : RTC句柄
 * @note        此函数会被HAL_RTC_Init()调用
 * @retval      无
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    uint16_t retry = 200;

    RCC_OscInitTypeDef rcc_osc_init_handle;
    RCC_PeriphCLKInitTypeDef rcc_periphclk_init_handle;

    __HAL_RCC_RTC_ENABLE();     /* RTC使能 */
    HAL_PWR_EnableBkUpAccess(); /* 取消备份区域写保护 */
    __HAL_RCC_PWR_CLK_ENABLE(); /* 使能电源时钟PWR */
    __HAL_RCC_RTC_ENABLE();     /* RTC使能 */

    /* 使用寄存器的方式去检测LSE是否可以正常工作 */
    RCC->BDCR |= 1 << 0;        /* 尝试开启LSE */

    while (retry && ((RCC->BDCR & 0x02) == 0))  /* 等待LSE准备好 */
    {
        retry--;
        delay_ms(5);
    }
    
    if (retry == 0)     /* LSE起振失败 使用LSI */
    {
        rcc_osc_init_handle.OscillatorType = RCC_OSCILLATORTYPE_LSI;  /* 选择要配置的振荡器 */
        rcc_osc_init_handle.LSIState = RCC_LSI_ON;                    /* LSI状态：开启 */
        rcc_osc_init_handle.PLL.PLLState = RCC_PLL_NONE;              /* PLL无配置 */
        HAL_RCC_OscConfig(&rcc_osc_init_handle);                      /* 配置设置的rcc_oscinitstruct */

        rcc_periphclk_init_handle.PeriphClockSelection = RCC_PERIPHCLK_RTC; /* 选择要配置外设 RTC */
        rcc_periphclk_init_handle.RTCClockSelection = RCC_RTCCLKSOURCE_LSI; /* RTC时钟源选择LSI */
        HAL_RCCEx_PeriphCLKConfig(&rcc_periphclk_init_handle);              /* 配置设置的rcc_periphclkinitstruct */
        rtc_write_bkr(0, 0x5051);
    }
    else
    {
        rcc_osc_init_handle.OscillatorType = RCC_OSCILLATORTYPE_LSE;        /* 选择要配置的振荡器 */
        rcc_osc_init_handle.PLL.PLLState = RCC_PLL_NONE;                    /* PLL不配置 */
        rcc_osc_init_handle.LSEState = RCC_LSE_ON;                          /* LSE状态:开启 */
        HAL_RCC_OscConfig(&rcc_osc_init_handle);                            /* 配置设置的rcc_oscinitstruct */

        rcc_periphclk_init_handle.PeriphClockSelection = RCC_PERIPHCLK_RTC; /* 选择要配置外设RTC */
        rcc_periphclk_init_handle.RTCClockSelection = RCC_RTCCLKSOURCE_LSE; /* RTC时钟源选择LSE */
        HAL_RCCEx_PeriphCLKConfig(&rcc_periphclk_init_handle);              /* 配置设置的rcc_periphclkinitstruct */
        rtc_write_bkr(0, 0x5050);
    }
}

/**
 * @breif       设置闹钟时间(按星期闹铃,24小时制)
 * @param       week        : 星期几(1~7) 
 * @param       hour,min,sec: 小时,分钟,秒钟
 * @retval      无
 */
void rtc_set_alarma(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec)
{
    RTC_AlarmTypeDef rtc_alarm_handle;
    
    rtc_alarm_handle.AlarmTime.Hours = hour;                                /* 小时 */
    rtc_alarm_handle.AlarmTime.Minutes = min;                               /* 分钟 */
    rtc_alarm_handle.AlarmTime.Seconds = sec;                               /* 秒 */
    rtc_alarm_handle.AlarmTime.SubSeconds = 0;
    rtc_alarm_handle.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
    
    rtc_alarm_handle.AlarmMask = RTC_ALARMMASK_NONE;                        /* 精确匹配星期, 时分秒 */
    rtc_alarm_handle.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    rtc_alarm_handle.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY; /* 按星期 */
    rtc_alarm_handle.AlarmDateWeekDay = week;                               /* 星期 */
    rtc_alarm_handle.Alarm = RTC_ALARM_A;                                   /* 闹钟A */
    HAL_RTC_SetAlarm_IT(&g_rtc_handle, &rtc_alarm_handle, RTC_FORMAT_BIN);
    
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 1, 2);   /* 抢占优先级1,子优先级2 */
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

/**
 * @breif       周期性唤醒定时器设置
 * @param       wksel
 *   @arg       RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
 *   @arg       RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
 *   @arg       RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
 *   @arg       RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
 *   @arg       RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
 *   @arg       RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)
 * @note        000,RTC/16;001,RTC/8;010,RTC/4;011,RTC/2;
 * @note        注意:RTC就是RTC的时钟频率,即RTCCLK!
 * @param       cnt: 自动重装载值.减到0,产生中断.
 * @retval      无
 */
void rtc_set_wakeup(uint8_t wksel, uint16_t cnt)
{ 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_WUTF);  /* 清除RTC WAKE UP的标志 */

    HAL_RTCEx_SetWakeUpTimer_IT(&g_rtc_handle, cnt, wksel);          /* 设置重装载值和时钟 */

    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 2, 2);                       /* 抢占优先级2, 子优先级2 */
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

/**
 * @breif       RTC闹钟中断服务函数
 * @param       无
 * @retval      无
 */
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&g_rtc_handle);
}

/**
 * @breif       RTC闹钟A中断处理回调函数
 * @param       hrtc : RTC句柄
 * @retval      无
 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    printf("ALARM A!\r\n");
}

/**
 * @breif       RTC WAKE UP中断服务函数
 * @param       无
 * @retval      无
 */
void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&g_rtc_handle); 
}

/**
 * @breif       RTC WAKE UP中断处理处理回调函数
 * @param       hrtc : RTC句柄
 * @retval      无
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    //LED1_TOGGLE();
	//printf("Wake up!\r\n");
}
