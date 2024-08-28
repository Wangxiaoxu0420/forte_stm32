/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015-2019, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-01-16
 */

#include <easyflash.h>
#include "malloc.h"
#include "shell_port.h"
#include "log.h"

extern EF_OPS_T *ef_ops;

uint32_t boot_count = 0;
static uint32_t boot_time = 0;
static uint32_t debug_level = 0;

/**
 * @brief easyflash porting layer
 */

/* default environment variables set for user */
static const ef_env default_env_set[] = {
//      {   key  , value, value_len }，
        {"username", "root", 0},                                           //类型为字符串的环境变量可以设定值的长度为 0 ，此时会在初始化时会自动检测其长度
        {"boot_count", &boot_count, sizeof(boot_count)},                   //整形
        {"boot_time",  &boot_time, sizeof(boot_time)},                     //数组类型，其他类型使用方式类似
        {"type",            "unknown",          0},                        //产品型号
        {"version",         "unknown",          0},                        //产品版本
        {"sn",              "unknown",          0},                        //产品序列号
        {"mqttip",          "192.168.43.1",     0},                        //IP地址
        {"mqttport",        "1883",             0},                        //IP地址
        {"mac",             NULL,               0},                        //MAC地址
        {"collectorid",     NULL,               0},                        //采集器ID
        {"debug_level",     &debug_level,       sizeof(debug_level)},      //调试等级
        {"soft_version",    "unknown",          0},                        //软件版本
        {"hardware_version","unknown",          0},                        //硬件版本
        {"log_level",       "ERROR",            0},                        //日志等级


};
uint16_t env_config(const ef_env *env_config, uint16_t index, uint16_t data)
{
    uint16_t max_len = 0;
    while (env_config[max_len].key != NULL)
    {
        max_len++;
        if(max_len == index)    break;  //找到了参数所在索引
        if(max_len == 0xFF)     return ~data;//参数最多不超过255个
    }

	if(index >= max_len)
		return ~data;//写地址不在本机范围内，则返回其他值
	ef_set_value(env_config[index].key, data);//更新到flash中
	*(uint32_t *)env_config[index].value = data;//更新当前内存值
	return 0;
}
EfErrCode ef_env_register(const ef_env *env) 
{
    EfErrCode result = EF_NO_ERR;
    struct env_node_obj env_r;
    uint32_t i;
    for (i = 0; env[i].key != NULL; i++) {
        //如果没有这个环境变量，则设置默认变量
        if( ef_get_env_obj(env[i].key, &env_r) == false ) {
            result = ef_set_env_blob(env[i].key, env[i].value, env[i].value_len);
        }
        else {
            //如果环境变量存在，则读取flash中存储的值到内存中
		    *(uint32_t *)env[i].value = ef_get_value(env[i].key);
        }
    }
    return result;
}
/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size) {
    EfErrCode result = EF_NO_ERR;

    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);

    return result;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;

    /* You can add your code under here. */
    ef_ops->read((uint8_t *)buf, addr, size);
    return result;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size) {
    EfErrCode result = EF_NO_ERR;

    /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
    EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);

    /* You can add your code under here. */
    for (size_t i = 0; i <= (size-1)/EF_ERASE_MIN_SIZE; i++)
    {
        //erase the flash sector which include the addr
        ef_ops->erase(addr/EF_ERASE_MIN_SIZE + i);
    }
    
    return result;
}
/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;
    
    /* You can add your code under here. */
    ef_ops->write((uint8_t *)buf, addr, size);
    return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void) {
    
    /* You can add your code under here. */
    
}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void) {
    
    /* You can add your code under here. */
    
}


/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...) {

#ifdef PRINT_DEBUG

    va_list args;

    // 确定需要的缓冲区大小
    size_t buffer_size = 256; // 初始缓冲区大小，视需要调整

    char *buffer = (char *)GWmalloc(buffer_size);
    if (buffer == NULL) {
        return;
    }
    
    // 可变参数格式化
    va_start(args, format);
    int log_len = vsnprintf(buffer, buffer_size, format, args);
    va_end(args);

    // 检查缓冲区是否足够大
    if (log_len < 0 || (size_t)(log_len) >= buffer_size) {
        buffer_size = log_len + 1;
        char *new_buffer = (char *)GWrealloc(buffer, buffer_size);
        if (new_buffer == NULL) {
            GWfree(buffer);
            return;
        }
        buffer = new_buffer;

        va_start(args, format);
        vsnprintf(buffer, buffer_size, format, args);
        va_end(args);
    }

    ef_log_write((uint32_t *)buffer, (buffer_size + 3) & ~3, ARER_LOG);
    GWfree(buffer);
#endif
}

/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...) {
    va_list args;
    printf(format);
    #if 0
    // 确定需要的缓冲区大小
    size_t buffer_size = 256; // 初始缓冲区大小，视需要调整

    char *buffer = (char *)GWmalloc(buffer_size);
    if (buffer == NULL) {
        return;
    }
    
    // 可变参数格式化
    va_start(args, format);
    int log_len = vsnprintf(buffer, buffer_size, format, args);
    va_end(args);

    // 检查缓冲区是否足够大
    if (log_len < 0 || (size_t)(log_len) >= buffer_size) {
        buffer_size = log_len + 1;
        char *new_buffer = (char *)GWrealloc(buffer, buffer_size);
        if (new_buffer == NULL) {
            GWfree(buffer);
            return;
        }
        buffer = new_buffer;

        va_start(args, format);
        vsnprintf(buffer, buffer_size, format, args);
        va_end(args);
    }

    ef_log_write((uint32_t *)buffer, (buffer_size + 3) & ~3, ARER_LOG);
    GWfree(buffer);
    #endif
}
/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...) {
    va_list args;

    printf(format);
    /* args point to the first variable parameter */
    #if 0
    va_start(args, format);
    /* You can add your code under here. */
    // 分配缓冲区
    char *buffer = (char *)GWmalloc(128);
    memset(buffer, 0, 128);
    if (buffer == NULL) {
        va_end(args);
        return;
    }
    vsnprintf(buffer, 128, format, args);
    /* You can add your code under here. */
    ef_log_write((uint32_t *)buffer, (128 + 3) & ~3, ARER_LOG);
    GWfree(buffer);
    va_end(args);
    #endif
}

void ef_log(const char *format, va_list args) {
#ifdef PRINT_DEBUG
    size_t buffer_size = 256; // 初始缓冲区大小，视需要调整
    char *buffer = (char *)GWmalloc(buffer_size);
    if (buffer == NULL) {
        return;
    }

    int log_len = vsnprintf(buffer, buffer_size, format, args);
    log_len = (log_len + 3) & ~3;// 对齐到4字节边界
    // 检查缓冲区是否足够大
    if (log_len < 0) {
        // 处理错误情况
        GWfree(buffer);
    }

    if ((size_t)log_len > buffer_size) {
        buffer_size = log_len;
        char *new_buffer = (char *)GWrealloc(buffer, buffer_size);
        if (new_buffer == NULL) {
            GWfree(buffer);
            return;
        }
        memset(new_buffer, 0, buffer_size);
        buffer = new_buffer;
        log_len = vsnprintf(buffer, buffer_size, format, args);
        log_len = (log_len + 3) & ~3;
        // 检查缓冲区是否足够大
        if (log_len < 0) {
            // 处理错误情况
            GWfree(buffer);
            return;
        }
    }
    //printf("%s",buffer); //数据记录到日志中
    shellWriteEndLine(&shell, buffer, strlen(buffer));
    ef_log_write((uint32_t *)buffer, log_len, ARER_LOG);
    GWfree(buffer);
#endif
}
void ef_data(const char *format, ...) {
#ifdef PRINT_DEBUG
    size_t buffer_size = 256; // 初始缓冲区大小，视需要调整
    char *buffer = (char *)GWmalloc(buffer_size);
    if (buffer == NULL) {
        return;
    }
    va_list args;
    va_start(args, format);

    int log_len = vsnprintf(buffer, buffer_size, format, args);
    log_len = (log_len + 3) & ~3; // 对齐到4字节边界
    // 检查缓冲区是否足够大
    if (log_len < 0) {
        // 处理错误情况
        GWfree(buffer);
    }
    memset(buffer, 0, buffer_size);
    if ((size_t)log_len > buffer_size) {
        buffer_size = log_len;
        char *new_buffer = (char *)GWrealloc(buffer, buffer_size);
        if (new_buffer == NULL) {
            GWfree(buffer);
            va_end(args);
            return;
        }
        memset(new_buffer, 0, buffer_size);
        buffer = new_buffer;
        log_len = vsnprintf(buffer, buffer_size, format, args);
        log_len = (log_len + 3) & ~3; // 对齐到4字节边界
        // 检查缓冲区是否足够大
        if (log_len < 0) {
            // 处理错误情况
            GWfree(buffer);
            va_end(args);
            return;
        }
    }
    //printf("%s",buffer); //数据记录到日志中
    ef_log_write((uint32_t *)buffer, log_len, ARER_DATA);
    GWfree(buffer);
    va_end(args);
    return;
#endif
}