/**
 * @file shell_fs.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell file system support
 * @version 0.1
 * @date 2020-07-22
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#ifndef __SHELL_FS_H__
#define __SHELL_FS_H__

#include "easyflash.h"

#define     SHELL_FS_VERSION                "1.0.0"

#define     SHELL_COMPANION_ID_FS           -1

#define     SHELL_FS_LIST_FILE_BUFFER_MAX   4096

/**
 * @brief shell文件系统支持结构体
 * 
 */
typedef struct shell_fs
{
    size_t (*set_value)(const char *key, uint32_t value);
    uint32_t (*get_value)(const char *key);
    void (*print_env)(void);
    char *(*get_env)(const char *key);
    EfErrCode (*set_env)(const char *key, const char *value);
    EfErrCode (*del_env)(const char *key);
    EfErrCode (*env_set_default)(void);
    EfErrCode (*log_read)(size_t index, uint32_t *log, size_t size, uint8_t area_select);
    EfErrCode (*log_clean)(uint8_t area_select);
    size_t (*log_get_used_size)(uint8_t area_select);
    size_t (*log_get_total_size)(uint8_t area_select);
} ShellFs;

void userShellFsInit(void);

#endif
