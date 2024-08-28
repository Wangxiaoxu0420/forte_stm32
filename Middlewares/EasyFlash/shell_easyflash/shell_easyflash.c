/**
 * @file shell_fs.c
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell file system support
 * @version 0.1
 * @date 2020-07-22
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#include "shell.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>
#include "./SYSTEM/delay/delay.h"

#include "easyflash.h"
#include "shell_easyflash.h"
#include "FreeRTOS.h"
#include "task.h"
#include "norflash.h"
#include "log.h"

extern Shell shell;
extern char shellBuffer[512];

ShellFs shellFs;

/**
 * @brief 用户shell初始化
 * 
 */
void userShellFsInit(void)
{
    shellFs.del_env = ef_del_env;
    shellFs.set_value = ef_set_value;
    shellFs.get_value = ef_get_value;
    shellFs.print_env = ef_print_env;
    shellFs.get_env = ef_get_env;
    shellFs.set_env = ef_set_env;
    shellFs.del_env = ef_del_env;
    shellFs.env_set_default = ef_env_set_default;
    shellFs.log_read = ef_log_read;
    shellFs.log_clean = ef_log_clean;
    shellFs.log_get_used_size = ef_log_get_used_size;
    shellFs.log_get_total_size = ef_log_get_total_size;

    shellInit(&shell, shellBuffer, 512);
    shellCompanionAdd(&shell, SHELL_COMPANION_ID_FS, &shellFs);
    shellPrint(&shell, "EasyFlash load filesystem success!\r\n");
}
/**
 * @brief       获取任务信息
 * @param       无
 * @retval      无
 */ 
void taskinfo(void)
{
    char InfoBuffer[512];
    vPortEnterCritical();
    vTaskList(InfoBuffer);
    vPortExitCritical();
    shellPrint(&shell, "\r\n任务名\t状态\t优先级\t剩余栈\t任务序号\r\n");
    shellPrint(&shell, "%s",InfoBuffer);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
					taskinfo, taskinfo, list current task and info);

void reboot(char *str)
{
    HAL_NVIC_SystemReset();
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
					reboot, reboot, reboot system);


/**
 * @brief 打印norflash中的内容
 * 
 */
void norflash_print(uint32_t addr)
{
    char InfoBuffer[512];
    norflash_read((uint8_t *)InfoBuffer, addr, 512);
    for (size_t i = 0; i < 512; i++)
    {
        LOG_I("%02X ", InfoBuffer[i]);
        if(i % 16 == 15)
            shellPrint(&shell, "\r\n");
    }    
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
					fprint, norflash_print, norflash_print);

/**
 * @brief 读flash指定地址
 * 
 */
void norflash_get(uint32_t addr)
{
    uint8_t data;
    norflash_read((uint8_t *)&data, addr, 1);
    shellPrint(&shell, "%02X", data);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
					fget, norflash_get, norflash_get);

/**
 * @brief 写flash指定地址
 * 
 */
void norflash_set(uint32_t addr, uint8_t data)
{
    norflash_write((uint8_t *)&data, addr, 1);
    shellPrint(&shell, "%02X", data);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
					fset, norflash_set, norflash_set);

/**
 * @brief 获取当前所有环境变量
 * 
 * @param 
 */
void shelleflist(void)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    //打印当前所有环境变量
    shellFs->print_env();
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
eflist, shelleflist, list all env show name/value&string/address/size);

/**
 * @brief 写一个字符串到环境变量
 * 
 * @param 
 */
void shellefvalue(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    uint32_t value = NULL;
    for (size_t argc_check = 0; argc_check < argc; argc_check++)
    {
        if(argv[argc_check] == NULL) 
            return 1;
    }
    LOG_I("%dparameter(s)", argc);
    switch (argc)
    {
    case 1:
        //打印当前所有环境变量
        shellFs->print_env();
        break;
    case 2:
        value = shellFs->get_value(argv[1]);
        shellPrint(shell, "[value]%s=%d(0x%x)\r\n", argv[1], value, value);
        break;
    case 3:
        value = shellFs->get_value(argv[1]);
        shellPrint(shell, "[last value]%s=%d(0x%x)\r\n", argv[1], value, value);
        shellFs->set_value(argv[1], atoi(argv[2]));
        shellPrint(shell, "[set value]%s=%d(0x%x)\r\n", argv[1], atoi(argv[2]), atoi(argv[2]));
        break;
    default:
        break;
    }

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), shellefvalue, shellefvalue, shellefvalue [valuestring] [value]);


/**
 * @brief 写一个字符串到环境变量
 * 
 * @param 
 */
void shellefstr(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    for (size_t argc_check = 0; argc_check < argc; argc_check++)
    {
        if(argv[argc_check] == NULL) 
            return 1;
    }
    LOG_I("%dparameter(s)", argc);
    switch (argc)
    {
    case 1:
        //打印当前所有环境变量
        shellFs->print_env();
        break;
    case 2:
        shellPrint(shell, "[value]%s=%s\r\n", argv[1], shellFs->get_env(argv[1]));
        break;
    case 3:
        shellPrint(shell, "[last string]%s=%s\r\n", argv[1], shellFs->get_env(argv[1]));
        shellFs->set_env(argv[1], argv[2]);
        shellPrint(shell, "[set string]%s=%s\r\n", argv[1], argv[2]);
        break;
    default:
        break;
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), efstr, shellefstr, shellefstr [valuestring] [value]);

/**
 * @brief 删除环境变量
 * 
 * @param 
 */
void shellefdelenv(const char *key)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    shellFs->del_env(key);
    shellPrint(shell, "[delkey]%s\r\n", key);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
efdel, shellefdelenv, delete a env of key);

/**
 * @brief 设置默认环境变量
 * 
 * @param 
 */
void shellefdefault(void)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    shellFs->env_set_default();
    shellPrint(shell, "[default]set default env success!\r\n");
    shellPrint(shell, "reset system after 3 seconds...!\r\n");
    delay_ms(3000);
    HAL_NVIC_SystemReset();
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
efdefault, shellefdefault, set all env to default and reboot system);

/**
 * @brief 写一个字符串到环境变量
 * 
 * @param 
 */
void shelliotype(const char *value)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    
    shellFs->set_env("IO_TYPE", value);
    shellPrint(shell, "IO_TYPE=%s\r\n", value);
    shellPrint(shell, "reset system after 3 seconds...!\r\n");
    delay_ms(3000);
    HAL_NVIC_SystemReset();
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
iotype, shelliotype, set IO_TYPE then trigger reboot to reinitialize);

/**
 * @brief 读取当前所有log输出到串口
 * 
 * @param 
 */
void shelllogread(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    
    char *buf = (char *)SHELL_MALLOC(512);
    uint32_t used_size = shellFs->log_get_used_size(ARER_LOG);
    uint32_t read_size = shellFs->log_get_used_size(ARER_LOG);
    size_t i = 0;
    if(argc == 2)
    {
        i = used_size - atoi(argv[1]);
    }
    else{
        i = 0;
    }
    for (; i < read_size; i += 512 )
    {
        if( (used_size - i) < 512) 
        {
            shellFs->log_read(i, (uint32_t *)buf, used_size - i, ARER_LOG);
            for (size_t j = 0; j < used_size-i; j++)
            {
                if( (buf[j]>=0x20 && buf[j]<=0x7E) || (buf[j]==0x0d) || (buf[j]==0x0a) )
                //按字节顺序输出，因为中间会有\0终止符，所以不能全给printf，否则会乱码，\0后面的会无法输出
                    shellPrint(shell, "%c", buf[j]);
            }
        }
        else
        {
            shellFs->log_read(i, (uint32_t *)buf, 512, ARER_LOG);
            for (size_t j = 0; j < 512; j++)
            {
                if( (buf[j]>=0x20 && buf[j]<=0x7E) || (buf[j]==0x0d) || (buf[j]==0x0a) )
                //按字节顺序输出，因为中间会有\0终止符，所以不能全给printf，否则会乱码，\0后面的会无法输出
                    shellPrint(shell, "%c", buf[j]);
            }
        }
        //如果串口有输入，则中断输出
        char data;
        if(shell->read(&data, 1) == 1)
        break;
    }
    SHELL_FREE(buf);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), lread, shelllogread, lread [valuestring] [value]);

/**
 * @brief 清楚所有log
 * 
 * @param 
 */
void shelllogclean(void)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    
    shellFs->log_clean(ARER_LOG);
    shellPrint(shell, "clean all log success!\r\n");
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
lclean, shelllogclean, clean all log);

/**
 * @brief 获取当前log使用情况
 * 
 * @param 
 */
void shellloginfo(void)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    size_t used_size = shellFs->log_get_used_size(ARER_LOG);
    size_t total_size = shellFs->log_get_total_size(ARER_LOG);
    shellPrint(shell, "log used :%d.%d%%. used size:%d. total size:%d. \r\n", used_size*100/total_size, used_size*10000/total_size%100, used_size, total_size);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
linfo, shellloginfo, show log info on flash);


/**
 * @brief 获取当前log使用情况，默认ERROR级别
 * 
 * @param     
    LOG_NONE,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_VERBOSE,
 */
void shellloglevel(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    char *level = NULL;
    char *level_str = NULL;
    for (size_t argc_check = 0; argc_check < argc; argc_check++)
    {
        if(argv[argc_check] == NULL) 
            return 1;
    }
    if(argc == 1)
    {
        shellPrint(shell, "log level is %s\r\n", log_level_str(shellFs->get_value("log_level")));
        return;
    }

    if( (strcmp(argv[1], "NONE") == 0)
    ||  (strcmp(argv[1], "none") == 0)
    ||  (strcmp(argv[1], "N") == 0)
    ||  (strcmp(argv[1], "n") == 0) )
    {
        level = LOG_NONE;
        level_str = "NONE";
    }
    if( (strcmp(argv[1], "ERROR") == 0)
    ||  (strcmp(argv[1], "error") == 0)
    ||  (strcmp(argv[1], "E") == 0)
    ||  (strcmp(argv[1], "e") == 0) )
    {
        level = LOG_ERROR;
        level_str = "ERROR";
    }
    if( (strcmp(argv[1], "WARN") == 0)
    ||  (strcmp(argv[1], "warn") == 0)
    ||  (strcmp(argv[1], "W") == 0)
    ||  (strcmp(argv[1], "w") == 0) )
    {
        level = LOG_WARN;
        level_str = "WARN";
    }
    if( (strcmp(argv[1], "INFO") == 0)
    ||  (strcmp(argv[1], "info") == 0)
    ||  (strcmp(argv[1], "I") == 0)
    ||  (strcmp(argv[1], "i") == 0) )
    {
        level = LOG_INFO;
        level_str = "INFO";
    }
    if( (strcmp(argv[1], "DEBUG") == 0)
    ||  (strcmp(argv[1], "debug") == 0)
    ||  (strcmp(argv[1], "D") == 0)
    ||  (strcmp(argv[1], "d") == 0) )
    {
        level = LOG_DEBUG;
        level_str = "DEBUG";
    }
    if( (strcmp(argv[1], "VERBOSE") == 0)
    ||  (strcmp(argv[1], "verbose") == 0)
    ||  (strcmp(argv[1], "V") == 0)
    ||  (strcmp(argv[1], "v") == 0) )
    {
        level = LOG_VERBOSE;
        level_str = "VERBOSE";
    }
    log_set_level(level);
    shellFs->set_value("log_level", level);
    shellPrint(shell, "log level set to %s\r\n", level_str);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), llevel, shellloglevel, shellloglevel [levelstring]);


/**
 * @brief 读取当前所有data输出到串口
 * 
 * @param 
 */
void shelldataread(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    
    char *buf = (char *)SHELL_MALLOC(512);
    uint32_t used_size = shellFs->log_get_used_size(ARER_DATA);
    size_t i = 0;
    if(argc == 2)
    {
        i = used_size - atoi(argv[1]);
    }
    else{
        i = 0;
    }
    for (; i < used_size; i += 512 )
    {
        if( (used_size - i) < 512) 
        {
            shellFs->log_read(i, (uint32_t *)buf, used_size - i, ARER_DATA);
            for (size_t j = 0; j < used_size-i; j++)
            {
                //按字节顺序输出，因为中间会有\0终止符，所以不能全给printf，否则会乱码，\0后面的会无法输出
                shellPrint(shell, "%c", buf[j]);
            }
        }
        else
        {
            shellFs->log_read(i, (uint32_t *)buf, 512, ARER_DATA);
            for (size_t j = 0; j < 512; j++)
            {
                //按字节顺序输出，因为中间会有\0终止符，所以不能全给printf，否则会乱码，\0后面的会无法输出
                shellPrint(shell, "%c", buf[j]);
            }
        }
        //如果串口有输入，则中断输出
        char data;
        if(shell->read(&data, 1) == 1)
            break;
    }
    SHELL_FREE(buf);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),dread, shelldataread, read all data to terminal);

/**
 * @brief 清楚所有data
 * 
 * @param 
 */
void shelldataclean(const char *value)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    
    shellFs->log_clean(ARER_DATA);
    shellPrint(shell, "clean all data success!\r\n");
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
dclean, shelldataclean, clean all data);

/**
 * @brief 获取当前data使用情况
 * 
 * @param 
 */
void shelldatainfo(const char *value)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    size_t used_size = shellFs->log_get_used_size(ARER_DATA);
    size_t total_size = shellFs->log_get_total_size(ARER_DATA);
    shellPrint(shell, "data used :%d.%d%%. used size:%d. total size:%d. \r\n", used_size*100/total_size, used_size*10000/total_size%100, used_size, total_size);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
dinfo, shelldatainfo, show data info on flash);

/**
 * @brief 获取最近的N条日志
 * 
 */


/**
 * @brief 获取当前内存使用率
 * 
 * @param 
 */
void shellmeminfo(const char *value)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    shellPrint(shell, "SRAM used: %d.%d%%, Free: %d Byte\r\n", my_mem_perused(SRAMIN)/10, my_mem_perused(SRAMIN)%10, MEM1_MAX_SIZE - my_mem_perused(SRAMIN)*MEM1_MAX_SIZE/1000);
    //获取freertos内存使用情况

    vTaskSuspendAll();
    size_t heap_remain = xPortGetFreeHeapSize();
    xTaskResumeAll();

    shellPrint(shell, "heap_remain: %d Byte\r\n", heap_remain);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
meminfo, shellmeminfo, show mem info);


/**
 * @brief 尝试分配内存，查看mem中剩余内存容量
 * 
 */
void shellmemtest(const char *value)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    void *p;
    for (size_t i = 1; i < 0xFFFF; i++)
    {
        p = SHELL_MALLOC(i * 128);
        if(p == NULL)
        {
            shellPrint(shell, "mem_malloc at least Free size:%d Byte\r\n", (i-1)*128);
            return;
        }
        SHELL_FREE(p);
    }
    shellPrint(shell, "mem_malloc sucess\r\n");
    return;
    
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
memtest, shellmemtest, show mem test);



