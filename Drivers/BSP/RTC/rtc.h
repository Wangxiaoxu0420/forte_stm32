#ifndef __RTC_H
#define __RTC_H

#include "./SYSTEM/sys/sys.h"


extern RTC_HandleTypeDef g_rtc_handle;

uint8_t rtc_init(void);                             /* 初始化RTC */
uint32_t rtc_read_bkr(uint32_t bkrx);               /* 读后备寄存器 */
void rtc_write_bkr(uint32_t bkrx, uint32_t data);   /* 写后备寄存器 */
void rtc_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *ampm);            /* 获取时间 */
HAL_StatusTypeDef rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm);   /* 设置时间 */
void rtc_get_date(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *week);         /* 获取日期 */
HAL_StatusTypeDef rtc_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week);/* 设置日期 */

void rtc_set_wakeup(uint8_t wksel, uint16_t cnt);                                       /* 设置周期性唤醒 */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);                        /* 获取星期 */
void rtc_set_alarma(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec);              /* 设置闹钟 */


void GetTimeInfo(struct tm *info);
void GetTimeStr(char *pcTime);
uint64_t GetTimeStamp(void);
void _UpdateTime(uint32_t Timestamp);

#endif
