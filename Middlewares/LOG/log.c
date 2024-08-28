#include <string.h>
#include <stdio.h>

#include "log.h"
#include "rtc.h"

#ifdef LOG_ENABLE

static log_level_t s_log_default_level = LOG_ERROR;

static vprintf_like_t s_log_print_func = &vprintf;

static getTick log_tick = NULL;
static getStr log_time_str = NULL;

/*
* @brief 设置打印输出回调函数
*/
vprintf_like_t log_set_output(vprintf_like_t func)
{
    vprintf_like_t orig_func = s_log_print_func;
    s_log_print_func = func;
    return orig_func;
}

/*
* @brief 设置日志输出等级
*/
void log_set_level(log_level_t level)
{
    s_log_default_level = level;
}

/*
* @brief 初始化log接口
*/
void log_init(log_level_t log,getStr func)
{
    s_log_default_level = log;
    //log_tick = func;
    log_time_str = func;
}

/*
* @brief 输出系统时间
*/
int log_get_tick(void)
{
    return log_tick();
}
char *log_get_str(void)
{
    return log_time_str();
}
/*
* @brief 判断是否允许输出日志
*/
static uint8_t should_output(log_level_t req,log_level_t def)
{
    return req<=def;
}

/*
* @brief 日志输出函数
*/
void log_write(log_level_t level,const char *format,...)
{
    va_list ap;
    va_start(ap,format);
    log_level_t level_for_tag = s_log_default_level;
    if(should_output(level,level_for_tag)){
       (*s_log_print_func)(format,ap); 
    }
    va_end(ap);
}

void LOG_BUFF_LL(char * tag,const char *funcName,int line,void *buff,uint8_t type,uint16_t len)
{
	char *ptr = (char *)buff;
	LOG_I("I (%08d) [%s] [%s-%d] Buffer content:\r\n",log_get_tick(),tag,funcName,line);
	for(int i=0; i<len; i++){
		LOG_I("%c",ptr[i]);
		if(i+1%16==0) LOG_I("\r\n");
	}
	LOG_I("\r\n");
}

char *log_level_str(log_level_t level)
{
    char *str = NULL;
    switch (level)
    {
    case LOG_NONE:
        str =  "NONE";
        break;
    case LOG_ERROR:
        str =  "ERROR";
        break;
    case LOG_WARN:
        str =  "WARN";
        break;
    case LOG_INFO:
        str = "INFO";
        break;
    case LOG_DEBUG:
        str = "DEBUG";
        break;
    case LOG_VERBOSE:
        str = "VERBOSE";
        break;
    
    default:
        break;
    }
    return str;
}

#else


#endif
