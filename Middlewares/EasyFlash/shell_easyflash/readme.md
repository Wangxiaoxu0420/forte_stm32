# letter shell file system support

![version](https://img.shields.io/badge/version-1.0.0-brightgreen.svg)
![standard](https://img.shields.io/badge/standard-c99-brightgreen.svg)
![build](https://img.shields.io/badge/build-2020.08.16-brightgreen.svg)
![license](https://img.shields.io/badge/license-MIT-brightgreen.svg)

letter shell文件系统支持

![preview](../../doc/img/fs_support_preview.gif)

- [letter shell file system support](#letter-shell-file-system-support)
  - [简介](#简介)
  - [使用](#使用)

## 简介

shell_easyflash是shell的一个插件，可以支持easyflash的env操作，包括set、get、del、print、env等命令。
可以在串口的shell中对flash进行操作。

在easyflash中已经定义好了文件系统初始化函数，为弱函数，所以只需要将shell_easyflash.c文件复制到工程中即可。

## 目录结构


```
shell_easyflash
├── shell_easyflash.h
└── shell_easyflash.c
```




## 使用

1. 声明`ShellFs`对象并申请当前路径数据缓冲区

    ```c
    ShellFs shellFs;
    ```

2. 接入easyflash库函数

    shellFs.ef_del_env = ef_del_env;
    shellFs.ef_set_value = ef_set_value;
    shellFs.ef_get_value = ef_get_value;
    shellFs.ef_print_env = ef_print_env;
    shellFs.ef_get_env = ef_get_env;
    shellFs.ef_set_env = ef_set_env;
    shellFs.ef_del_env = ef_del_env;
    shellFs.ef_env_set_default = ef_env_set_default;


3. 初始化`Shell`对象

    设置shell当前路径缓冲，初始化shell，并添加伴生对象

    ```C
    shellInit(&shell, shellBuffer, 512);
    shellCompanionAdd(&shell, SHELL_COMPANION_ID_FS, &shellFs);
    ```
