#ifndef __LOG_H
#define __LOG_H

#include <stdint.h>
#include <stdarg.h>


#ifndef LOG_ENABLE
#define LOG_ENABLE
#endif

/*
* @brief Log level define
*/
typedef enum{
    LOG_NONE,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_VERBOSE,
}log_level_t;
char *log_level_str(log_level_t level);


typedef int (*vprintf_like_t)(const char *,va_list);

#ifdef LOG_ENABLE
#define LOG_LOCAL_LEVEL LOG_INFO

#define DBG_FORMAT(format)  "%c %s [%s] [%s - %d]: " format "\r\n"

#define DBG_LOG_LEVEL(tag,level,format,...) do{\
            if(level==LOG_ERROR) log_write(LOG_ERROR, DBG_FORMAT(format),'E',log_get_str(),tag,__func__,__LINE__,##__VA_ARGS__);\
            else if(level==LOG_WARN) log_write(LOG_WARN, DBG_FORMAT(format),'W',log_get_str(),tag,__func__,__LINE__,##__VA_ARGS__);\
            else if(level==LOG_INFO) log_write(LOG_INFO, DBG_FORMAT(format),'I',log_get_str(),tag,__func__,__LINE__,##__VA_ARGS__);\
            else if(level==LOG_DEBUG) log_write(LOG_DEBUG, DBG_FORMAT(format),'D',log_get_str(),tag,__func__,__LINE__,##__VA_ARGS__);\
            else if(level==LOG_VERBOSE) log_write(LOG_VERBOSE, DBG_FORMAT(format),'V',log_get_str(),tag,__func__,__LINE__,##__VA_ARGS__);\
        }while(0)

#define LOG_E(format,...)   DBG_LOG_LEVEL(__FILE__,LOG_ERROR,format,##__VA_ARGS__)
#define LOG_W(format,...)   DBG_LOG_LEVEL(__FILE__,LOG_WARN,format,##__VA_ARGS__)
#define LOG_I(format,...)   DBG_LOG_LEVEL(__FILE__,LOG_INFO,format,##__VA_ARGS__)
#define LOG_D(format,...)   DBG_LOG_LEVEL(__FILE__,LOG_DEBUG,format,##__VA_ARGS__)
#define LOG_V(format,...)   DBG_LOG_LEVEL(__FILE__,LOG_VERBOSE,format,##__VA_ARGS__)

#define LOG_BUFF_CHAR(tag,buff,len)      LOG_BUFF_LL(tag,__func__,__LINE__,buff,0,len)
#define LOG_BUFF_INT(tag,buff,len)      LOG_BUFF_LL(tag,__func__,__LINE__,buff,1,len)
#define LOG_BUFF_LONG(tag,buff,len)      LOG_BUFF_LL(tag,__func__,__LINE__,buff,2,len)

vprintf_like_t log_set_output(vprintf_like_t func);

typedef int (*getTick)(void);
typedef char *(*getStr)(void);

void log_init(log_level_t log,getStr func);
int  log_get_tick(void);
char *log_get_str(void);
void log_set_level(log_level_t level);
void LOG_BUFF_LL(char * tag,const char *funcName,int line,void *buff,uint8_t type,uint16_t len);
void log_write(log_level_t level,const char *format,...);

#endif

#endif
