#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* 澶存枃锟?? */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include <stdint.h>

extern uint32_t SystemCoreClock;

/* 鍩猴拷?锟介厤缃?閫夐」 */
#define configUSE_PREEMPTION                            1                       /* 1: 鎶㈠崰寮忚皟搴﹀櫒, 0: 鍚堜綔寮忚皟搴﹀櫒, 鏃犻粯璁ら渶瀹氫箟 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION         1                       /* 1: 浣跨敤锟??浠讹拷?锟界畻涓嬩竴锟??瑕佽繍琛岀殑浠诲姟, 0: 浣跨敤锟??浠剁畻娉曪拷?锟界畻涓嬩竴锟??瑕佽繍琛岀殑浠诲姟, 榛橈拷??: 0 */
#define configUSE_TICKLESS_IDLE                         0                       /* 1: 浣胯兘tickless浣庡姛鑰楁ā锟??, 榛橈拷??: 0 */
#define configCPU_CLOCK_HZ                              SystemCoreClock         /* 瀹氫箟CPU涓伙拷??, 鍗曚綅: Hz, 鏃犻粯璁ら渶瀹氫箟 */
// #define configSYSTICK_CLOCK_HZ                          (configCPU_CLOCK_HZ / 8)/* 瀹氫箟SysTick鏃堕挓棰戠巼锛屽綋SysTick鏃堕挓棰戠巼涓庡唴鏍告椂閽燂拷?锟界巼涓嶅悓鏃舵墠锟??浠ュ畾锟??, 鍗曚綅: Hz, 榛橈拷??: 涓嶅畾锟?? */
#define configTICK_RATE_HZ                              1000                    /* 瀹氫箟绯荤粺鏃堕挓鑺傛媿棰戠巼, 鍗曚綅: Hz, 鏃犻粯璁ら渶瀹氫箟 */
#define configMAX_PRIORITIES                            32                      /* 瀹氫箟鏈€澶т紭鍏堢骇锟??, 鏈€澶т紭鍏堢骇=configMAX_PRIORITIES-1, 鏃犻粯璁ら渶瀹氫箟 */
#define configMINIMAL_STACK_SIZE                        128                     /* 瀹氫箟绌洪棽浠诲姟鐨勬爤绌洪棿澶у皬, 鍗曚綅: Byte, 鏃犻粯璁ら渶瀹氫箟 */
#define configMAX_TASK_NAME_LEN                         16                      /* 瀹氫箟浠诲姟鍚嶆渶澶у瓧绗︽暟, 榛橈拷??: 16 */
#define configUSE_16_BIT_TICKS                          0                       /* 1: 瀹氫箟绯荤粺鏃堕挓鑺傛媿璁℃暟鍣ㄧ殑鏁版嵁绫诲瀷锟??16浣嶆棤绗﹀彿锟??, 鏃犻粯璁ら渶瀹氫箟 */
#define configIDLE_SHOULD_YIELD                         1                       /* 1: 浣胯兘鍦ㄦ姠鍗犲紡璋冨害锟??,鍚屼紭鍏堢骇鐨勪换鍔¤兘鎶㈠崰绌洪棽浠诲姟, 榛橈拷??: 1 */
#define configUSE_TASK_NOTIFICATIONS                    1                       /* 1: 浣胯兘浠诲姟闂寸洿鎺ョ殑娑堟伅浼狅拷?,鍖呮嫭淇″彿閲忋€佷簨浠舵爣蹇楃粍鍜屾秷锟??锟??锟??, 榛橈拷??: 1 */
#define configTASK_NOTIFICATION_ARRAY_ENTRIES           1                       /* 瀹氫箟浠诲姟閫氱煡鏁扮粍鐨勫ぇ锟??, 榛橈拷??: 1 */
#define configUSE_MUTEXES                               1                       /* 1: 浣胯兘浜掓枼淇″彿锟??, 榛橈拷??: 0 */
#define configUSE_RECURSIVE_MUTEXES                     1                       /* 1: 浣胯兘閫掑綊浜掓枼淇″彿锟??, 榛橈拷??: 0 LWIP*/
#define configUSE_COUNTING_SEMAPHORES                   1                       /* 1: 浣胯兘璁℃暟淇″彿锟??, 榛橈拷??: 0 */
#define configUSE_ALTERNATIVE_API                       0                       /* 宸插純锟??!!! */
#define configQUEUE_REGISTRY_SIZE                       8                       /* 瀹氫箟锟??浠ユ敞鍐岀殑淇″彿閲忓拰娑堟伅闃熷垪鐨勪釜锟??, 榛橈拷??: 0 */
#define configUSE_QUEUE_SETS                            1                       /* 1: 浣胯兘娑堟伅闃熷垪, 榛橈拷??: 0 */
#define configUSE_TIME_SLICING                          1                       /* 1: 浣胯兘鏃堕棿鐗囪皟锟??, 榛橈拷??: 1 */
#define configUSE_NEWLIB_REENTRANT                      0                       /* 1: 浠诲姟鍒涘缓鏃跺垎閰峃ewlib鐨勯噸鍏ョ粨鏋勪綋, 榛橈拷??: 0 */
#define configENABLE_BACKWARD_COMPATIBILITY             1                       /* 1: 浣胯兘鍏硷拷?锟借€佺増锟??, 榛橈拷??: 1 LWIP*/
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS         3                       /* 瀹氫箟绾跨▼灞€閮ㄦ寚閽堝彉閲忕殑锟??锟??, 榛橈拷??: 0 */
#define configSTACK_DEPTH_TYPE                          uint16_t                /* 瀹氫箟浠诲姟鍫嗘爤娣卞害鐨勬暟锟??绫诲瀷, 榛橈拷??: uint16_t */
#define configMESSAGE_BUFFER_LENGTH_TYPE                size_t                  /* 瀹氫箟娑堟伅缂撳啿鍖轰腑姣忔潯娑堟伅鐨勯暱锟??, 榛橈拷??: size_t */

/* 鍐呭瓨鍒嗛厤鐩稿叧瀹氫箟 */
#define configSUPPORT_STATIC_ALLOCATION                 0                       /* 1: 锟??鎸侀潤鎬佺敵璇峰唴锟??, 榛橈拷??: 0 */
#define configSUPPORT_DYNAMIC_ALLOCATION                1                       /* 1: 锟??鎸佸姩鎬佺敵璇峰唴锟??, 榛橈拷??: 1 */
#define configTOTAL_HEAP_SIZE                           ((size_t)(60 * 1024))   /* FreeRTOS鍫嗕腑锟??鐢ㄧ殑RAM鎬婚噺, 鍗曚綅: Byte, 鏃犻粯璁ら渶瀹氫箟 */
#define configAPPLICATION_ALLOCATED_HEAP                0                       /* 1: 鐢ㄦ埛鎵嬪姩鍒嗛厤FreeRTOS鍐呭瓨锟??(ucHeap), 榛橈拷??: 0 */
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP       0                       /* 1: 鐢ㄦ埛锟??琛屽疄鐜颁换鍔″垱寤烘椂浣跨敤鐨勫唴瀛樼敵璇蜂笌閲婃斁鍑芥暟, 榛橈拷??: 0 */

/* 閽╁瓙鍑芥暟鐩稿叧瀹氫箟 */
#define configUSE_IDLE_HOOK                             0                       /* 1: 浣胯兘绌洪棽浠诲姟閽╁瓙鍑芥暟, 鏃犻粯璁ら渶瀹氫箟  */
#define configUSE_TICK_HOOK                             0                       /* 1: 浣胯兘SysTick锟??锟??閽╁瓙鍑芥暟, 鏃犻粯璁ら渶瀹氫箟 */
#define configCHECK_FOR_STACK_OVERFLOW                  0                       /* 1: 浣胯兘鏍堟孩鍑猴拷?锟芥祴鏂癸拷?1, 2: 浣胯兘鏍堟孩鍑猴拷?锟芥祴鏂癸拷?2, 榛橈拷??: 0 */
#define configUSE_MALLOC_FAILED_HOOK                    0                       /* 1: 浣胯兘鍔ㄦ€佸唴瀛樼敵璇峰け璐ラ挬瀛愬嚱锟??, 榛橈拷??: 0 */
#define configUSE_DAEMON_TASK_STARTUP_HOOK              0                       /* 1: 浣胯兘瀹氭椂鍣ㄦ湇鍔′换鍔★拷?锟斤拷?锟芥墽琛屽墠鐨勯挬瀛愬嚱锟??, 榛橈拷??: 0 */

/* 杩愶拷?锟芥椂闂村拰浠诲姟鐘舵€佺粺璁＄浉鍏冲畾锟?? */
#define configGENERATE_RUN_TIME_STATS                   1                       /* 1: 浣胯兘浠诲姟杩愶拷?锟界姸鎬佸弬鏁扮粺锟??, 榛橈拷??: 0 */
#if configGENERATE_RUN_TIME_STATS
#include "./BSP/TIMER/btim.h"
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()        ConfigureTimeForRunTimeStats()
extern uint32_t FreeRTOSRunTimeTicks;
#define portGET_RUN_TIME_COUNTER_VALUE()                FreeRTOSRunTimeTicks
#endif
#define configUSE_TRACE_FACILITY                        1                       /* 1: 浣胯兘锟??瑙嗗寲璺熻釜璋冭瘯, 榛橈拷??: 0 */
#define configUSE_STATS_FORMATTING_FUNCTIONS            1                       /* 1: configUSE_TRACE_FACILITY锟??1鏃讹紝浼氱紪璇憊TaskList()鍜寁TaskGetRunTimeStats()鍑芥暟, 榛橈拷??: 0 */

/* 鍗忕▼鐩稿叧瀹氫箟 */
#define configUSE_CO_ROUTINES                           0                       /* 1: 浣胯兘鍚堜綔寮忚皟搴︾浉鍏冲嚱锟??, 榛橈拷??: 0 */
#define configMAX_CO_ROUTINE_PRIORITIES                 2                       /* 瀹氫箟鐢ㄦ埛锟??浣跨敤鐨勬渶澶у悎浣滃紡浠诲姟浼樺厛锟??, 鏈€澶т紭鍏堢骇=configMAX_CO_ROUTINE_PRIORITIES-1, 鏃犻粯锟??configUSE_CO_ROUTINES锟??1鏃堕渶瀹氫箟 */

/* 锟??浠跺畾鏃跺櫒鐩稿叧瀹氫箟 */
#define configUSE_TIMERS                                1                               /* 1: 浣胯兘锟??浠跺畾鏃跺櫒, 榛橈拷??: 0 */
#define configTIMER_TASK_PRIORITY                       ( configMAX_PRIORITIES - 1 )    /* 瀹氫箟锟??浠跺畾鏃跺櫒浠诲姟鐨勪紭鍏堢骇, 鏃犻粯锟??configUSE_TIMERS锟??1鏃堕渶瀹氫箟 */
#define configTIMER_QUEUE_LENGTH                        5                               /* 瀹氫箟锟??浠跺畾鏃跺櫒鍛戒护闃熷垪鐨勯暱锟??, 鏃犻粯锟??configUSE_TIMERS锟??1鏃堕渶瀹氫箟 */
#define configTIMER_TASK_STACK_DEPTH                    ( configMINIMAL_STACK_SIZE * 2) /* 瀹氫箟锟??浠跺畾鏃跺櫒浠诲姟鐨勬爤绌洪棿澶у皬, 鏃犻粯锟??configUSE_TIMERS锟??1鏃堕渶瀹氫箟 */

/* 锟??閫夊嚱锟??, 1: 浣胯兘 */
#define INCLUDE_vTaskPrioritySet                        1                       /* 璁剧疆浠诲姟浼樺厛锟?? */
#define INCLUDE_uxTaskPriorityGet                       1                       /* 鑾峰彇浠诲姟浼樺厛锟?? */
#define INCLUDE_vTaskDelete                             1                       /* 鍒犻櫎浠诲姟 */
#define INCLUDE_vTaskSuspend                            1                       /* 鎸傝捣浠诲姟 */
#define INCLUDE_xResumeFromISR                          1                       /* 鎭?锟??锟藉湪锟??锟??锟??鎸傝捣鐨勪换锟?? */
#define INCLUDE_vTaskDelayUntil                         1                       /* 浠诲姟缁濓拷?锟藉欢锟?? */
#define INCLUDE_vTaskDelay                              1                       /* 浠诲姟寤舵椂 */
#define INCLUDE_xTaskGetSchedulerState                  1                       /* 鑾峰彇浠诲姟璋冨害鍣ㄧ姸锟?? */
#define INCLUDE_xTaskGetCurrentTaskHandle               1                       /* 鑾峰彇褰撳墠浠诲姟鐨勪换鍔″彞锟?? */
#define INCLUDE_uxTaskGetStackHighWaterMark             1                       /* 鑾峰彇浠诲姟鍫嗘爤鍘嗗彶鍓╀綑鏈€灏忥拷? */
#define INCLUDE_xTaskGetIdleTaskHandle                  1                       /* 鑾峰彇绌洪棽浠诲姟鐨勪换鍔″彞锟?? */
#define INCLUDE_eTaskGetState                           1                       /* 鑾峰彇浠诲姟鐘讹拷? */
#define INCLUDE_xEventGroupSetBitFromISR                1                       /* 鍦ㄤ腑锟??锟??璁剧疆浜嬩欢鏍囧織锟?? */
#define INCLUDE_xTimerPendFunctionCall                  1                       /* 灏嗗嚱鏁扮殑鎵э拷?锟芥寕鍒板畾鏃跺櫒鏈嶅姟浠诲姟 */
#define INCLUDE_xTaskAbortDelay                         1                       /* 锟??锟??浠诲姟寤舵椂 */
#define INCLUDE_xTaskGetHandle                          1                       /* 閫氳繃浠诲姟鍚嶈幏鍙栦换鍔″彞锟?? */
#define INCLUDE_xTaskResumeFromISR                      1                       /* 鎭?锟??锟藉湪锟??锟??锟??鎸傝捣鐨勪换锟?? */

/* 锟??锟??宓岋拷?锟斤拷?锟戒负閰嶇疆 */
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS 4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15                  /* 锟??锟??鏈€浣庝紭鍏堢骇 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5                   /* FreeRTOS锟??绠＄悊鐨勬渶楂樹腑锟??浼樺厛锟?? */
#define configKERNEL_INTERRUPT_PRIORITY                 ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_API_CALL_INTERRUPT_PRIORITY           configMAX_SYSCALL_INTERRUPT_PRIORITY

/* FreeRTOS锟??锟??鏈嶅姟鍑芥暟鐩稿叧瀹氫箟 */
#define xPortPendSVHandler                              PendSV_Handler
#define vPortSVCHandler                                 SVC_Handler

/* 锟??瑷€ */
#define vAssertCalled(char, int) printf("Error: %s, %d\r\n", char, int)
#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )

//flash根目录
#define ffconfigDEV_PATH "/"
//在大多数情况下， FAT 表在磁盘上有两个相同的副本， 可以在读取错误的情况下使用第二个副本。 如果：
//设置为 1，即可使用两个 FAT，效率较低，但更 安全。
//设置为 0，即可仅使用一个 FAT，第二个 FAT 将永远不会被写入。
#define ffconfigWRITE_BOTH_FATS    1
//设置为 1，即可维护文件和目录时间戳以进行创建、修改 和最后访问。
//设置为 0，即可排除 时间戳。
//如果使用时间支持，则必须提供以下函数：
//time_t FreeRTOS_time( time_t *pxTime );	
//与标准 time() 函数语义相同的 FreeRTOS_time。
#define ffconfigTIME_SUPPORT    0
//设置为 1，即可维护当前各项任务的工作目录 (CWD)， 该目录可访问文件系统，允许使用相对路径。
//设置为 0，即可禁用 CWD，在这种情况下，必须使用完整路径 访问每个文件。
//#define ffconfigHAS_CWD    1
//#define ffconfigMKDIR_RECURSIVE 1 
#define FF_PRINTF    printf
#define ffconfigLFN_SUPPORT 1


/* FreeRTOS MPU 鐗规畩瀹氫箟 */
//#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0
//#define configTOTAL_MPU_REGIONS                                8
//#define configTEX_S_C_B_FLASH                                  0x07UL
//#define configTEX_S_C_B_SRAM                                   0x07UL
//#define configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY            1
//#define configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS             1

/* ARMv8-M 瀹夊叏渚э拷??鍙ｇ浉鍏冲畾涔夛拷? */
//#define secureconfigMAX_SECURE_CONTEXTS         5

#endif /* FREERTOS_CONFIG_H */
