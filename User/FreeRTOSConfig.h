#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* 澶存枃锟?? */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include <stdint.h>

extern uint32_t SystemCoreClock;

/* 鍩猴�??锟介厤缃?�?夐�? */
#define configUSE_PREEMPTION                            1                       /* 1: 鎶㈠崰�??忚皟搴﹀�?, 0: 鍚堜綔�??忚皟搴﹀�?, 鏃犻�?璁ら渶瀹氫�? */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION         1                       /* 1: 浣跨敤锟??浠�?�拷?锟界畻涓嬩�?�锟??瑕佽繍琛岀殑浠诲�??, 0: 浣跨敤锟??浠剁畻娉�?�??锟界畻涓嬩�?�锟??瑕佽繍琛岀殑浠诲�??, 榛橈�???: 0 */
#define configUSE_TICKLESS_IDLE                         0                       /* 1: 浣胯兘tickless浣庡姛鑰楁ā锟??, 榛橈�???: 0 */
#define configCPU_CLOCK_HZ                              SystemCoreClock         /* 瀹氫箟CPU涓伙�???, 鍗曚�?: Hz, 鏃犻�?璁ら渶瀹氫�? */
// #define configSYSTICK_CLOCK_HZ                          (configCPU_CLOCK_HZ / 8)/* 瀹氫箟SysTick鏃堕挓棰戠巼锛屽綋SysTick鏃堕挓棰戠巼涓庡唴鏍告�?�閽燂拷?锟界巼涓嶅悓鏃舵墠锟??浠ュ畾锟??, 鍗曚�?: Hz, 榛橈�???: 涓嶅畾锟?? */
#define configTICK_RATE_HZ                              1000                    /* 瀹氫箟绯荤粺鏃堕挓鑺傛�?�棰戠巼, 鍗曚�?: Hz, 鏃犻�?璁ら渶瀹氫�? */
#define configMAX_PRIORITIES                            32                      /* 瀹氫箟鏈�?澶т紭鍏堢骇锟??, 鏈€澶т紭鍏堢�?=configMAX_PRIORITIES-1, 鏃犻�?璁ら渶瀹氫�? */
#define configMINIMAL_STACK_SIZE                        128                     /* 瀹氫箟绌�?棽浠诲�?�鐨�?爤绌�?棿澶у�?, 鍗曚�?: Byte, 鏃犻�?璁ら渶瀹氫�? */
#define configMAX_TASK_NAME_LEN                         16                      /* 瀹氫箟浠诲�?�鍚嶆渶澶у瓧绗︽�?, 榛橈�???: 16 */
#define configUSE_16_BIT_TICKS                          0                       /* 1: 瀹氫箟绯荤粺鏃堕挓鑺傛�?�璁℃暟鍣ㄧ殑鏁版嵁�?诲瀷锟??16浣嶆棤绗﹀彿锟??, 鏃犻�?璁ら渶瀹氫�? */
#define configIDLE_SHOULD_YIELD                         1                       /* 1: 浣胯兘鍦ㄦ�?�鍗犲紡璋冨害锟??,鍚屼�?鍏堢骇鐨�?换鍔¤兘鎶㈠崰绌洪棽浠诲�??, 榛橈�???: 1 */
#define configUSE_TASK_NOTIFICATIONS                    1                       /* 1: 浣胯兘浠诲�?�闂寸洿鎺ョ殑娑堟伅浼狅�??,鍖呮�?淇″彿閲忋�?佷簨浠舵爣蹇楃粍鍜屾秷锟??�???�???, 榛橈�???: 1 */
#define configTASK_NOTIFICATION_ARRAY_ENTRIES           1                       /* 瀹氫箟浠诲�?�閫氱煡鏁扮粍鐨�?ぇ锟??, 榛橈�???: 1 */
#define configUSE_MUTEXES                               1                       /* 1: 浣胯兘浜掓枼淇″彿�???, 榛橈�???: 0 */
#define configUSE_RECURSIVE_MUTEXES                     1                       /* 1: 浣胯兘閫掑綊浜掓枼淇″彿�???, 榛橈�???: 0 LWIP*/
#define configUSE_COUNTING_SEMAPHORES                   1                       /* 1: 浣胯兘璁℃暟淇″彿�???, 榛橈�???: 0 */
#define configUSE_ALTERNATIVE_API                       0                       /* 宸插純锟??!!! */
#define configQUEUE_REGISTRY_SIZE                       8                       /* 瀹氫箟锟??浠ユ敞鍐岀殑淇″彿閲忓拰娑堟伅闃熷�?鐨勪釜锟??, 榛橈�???: 0 */
#define configUSE_QUEUE_SETS                            1                       /* 1: 浣胯兘娑堟伅闃熷�?, 榛橈�???: 0 */
#define configUSE_TIME_SLICING                          1                       /* 1: 浣胯兘鏃堕棿鐗囪皟锟??, 榛橈�???: 1 */
#define configUSE_NEWLIB_REENTRANT                      0                       /* 1: 浠�?��?�鍒涘缓鏃跺垎閰峃ewlib鐨勯噸鍏ョ粨鏋勪�?, 榛橈�???: 0 */
#define configENABLE_BACKWARD_COMPATIBILITY             1                       /* 1: 浣胯兘鍏硷拷?锟借€佺�?�锟??, 榛橈�???: 1 LWIP*/
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS         3                       /* 瀹氫箟绾跨▼灞€�?ㄦ寚閽堝彉閲忕殑�???�???, 榛橈�???: 0 */
#define configSTACK_DEPTH_TYPE                          uint16_t                /* 瀹氫箟浠诲�?�鍫嗘爤娣卞害鐨�?暟锟??�?诲�?, 榛橈�???: uint16_t */
#define configMESSAGE_BUFFER_LENGTH_TYPE                size_t                  /* 瀹氫箟娑堟伅缂撳啿鍖轰腑姣忔�?娑堟伅鐨�?暱锟??, 榛橈�???: size_t */

/* 鍐呭瓨鍒嗛厤鐩�?�叧瀹氫�? */
#define configSUPPORT_STATIC_ALLOCATION                 0                       /* 1: �???鎸侀潤鎬佺敵璇峰唴锟??, 榛橈�???: 0 */
#define configSUPPORT_DYNAMIC_ALLOCATION                1                       /* 1: �???鎸佸姩鎬佺敵璇峰唴锟??, 榛橈�???: 1 */
#define configTOTAL_HEAP_SIZE                           ((size_t)(70 * 1024))   /* FreeRTOS�?嗕腑�???�?ㄧ殑RAM�?婚噺, 鍗曚�?: Byte, 鏃犻�?璁ら渶瀹氫�? */
#define configAPPLICATION_ALLOCATED_HEAP                0                       /* 1: �?ㄦ埛鎵�??姩鍒嗛厤FreeRTOS鍐呭瓨锟??(ucHeap), 榛橈�???: 0 */
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP       0                       /* 1: �?ㄦ埛�???琛屽疄鐜颁换鍔″垱寤烘椂浣跨敤鐨勫唴瀛�?�敵璇蜂笌閲婃斁鍑芥�?, 榛橈�???: 0 */

/* 閽╁瓙鍑芥暟鐩�?�叧瀹氫�? */
#define configUSE_IDLE_HOOK                             0                       /* 1: 浣胯兘绌�?棽浠诲�?�閽╁瓙鍑芥�?, 鏃犻�?璁ら渶瀹氫�?  */
#define configUSE_TICK_HOOK                             0                       /* 1: 浣胯兘SysTick�???�???閽╁瓙鍑芥暟, 鏃犻�?璁ら渶瀹氫�? */
#define configCHECK_FOR_STACK_OVERFLOW                  0                       /* 1: 浣胯兘鏍堟�?�鍑猴拷?锟芥祴鏂癸拷?1, 2: 浣胯兘鏍堟�?�鍑猴拷?锟芥祴鏂癸拷?2, 榛橈�???: 0 */
#define configUSE_MALLOC_FAILED_HOOK                    0                       /* 1: 浣胯兘鍔ㄦ€佸唴瀛�?�敵璇峰け璐ラ挬瀛愬嚱锟??, 榛橈�???: 0 */
#define configUSE_DAEMON_TASK_STARTUP_HOOK              0                       /* 1: 浣胯兘瀹氭椂鍣ㄦ湇鍔′换鍔★�??锟斤�??锟芥墽琛屽�?�鐨�?�?瀛愬嚱锟??, 榛橈�???: 0 */

/* 杩愶�??锟芥椂闂村拰浠�?��?�鐘舵€佺粺璁＄浉鍏冲畾�??? */
#define configGENERATE_RUN_TIME_STATS                   1                       /* 1: 浣胯兘浠诲�?�杩愶拷?锟界姸鎬佸弬鏁扮粺锟??, 榛橈�???: 0 */
#if configGENERATE_RUN_TIME_STATS
#include "./BSP/TIMER/btim.h"
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()        ConfigureTimeForRunTimeStats()
extern uint32_t FreeRTOSRunTimeTicks;
#define portGET_RUN_TIME_COUNTER_VALUE()                FreeRTOSRunTimeTicks
#endif
#define configUSE_TRACE_FACILITY                        1                       /* 1: 浣胯兘锟??瑙嗗寲璺熻釜璋冭�?, 榛橈�???: 0 */
#define configUSE_STATS_FORMATTING_FUNCTIONS            1                       /* 1: configUSE_TRACE_FACILITY�???1鏃�?�紝浼氱�?璇憊TaskList()鍜寁TaskGetRunTimeStats()鍑芥�?, 榛橈�???: 0 */

/* 鍗忕▼鐩稿叧瀹氫�? */
#define configUSE_CO_ROUTINES                           0                       /* 1: 浣胯兘鍚堜綔�?忚皟搴︾浉鍏冲嚱�???, 榛橈�???: 0 */
#define configMAX_CO_ROUTINE_PRIORITIES                 2                       /* 瀹氫箟鐢ㄦ埛�???浣跨敤鐨�?渶澶у悎浣滃紡浠�?��?�浼樺厛�???, 鏈€澶т紭鍏堢�?=configMAX_CO_ROUTINE_PRIORITIES-1, 鏃犻�?�???configUSE_CO_ROUTINES�???1鏃堕渶瀹氫�? */

/* �???浠跺畾鏃跺櫒鐩�?�叧瀹氫�? */
#define configUSE_TIMERS                                1                               /* 1: 浣胯兘锟??浠跺畾鏃跺櫒, 榛橈�???: 0 */
#define configTIMER_TASK_PRIORITY                       ( configMAX_PRIORITIES - 1 )    /* 瀹氫箟锟??浠跺畾鏃跺櫒浠�?��?�鐨�?�?鍏堢�?, 鏃犻�?�???configUSE_TIMERS�???1鏃堕渶瀹氫�? */
#define configTIMER_QUEUE_LENGTH                        5                               /* 瀹氫箟锟??浠跺畾鏃跺櫒鍛戒护闃熷垪鐨勯暱锟??, 鏃犻�?�???configUSE_TIMERS�???1鏃堕渶瀹氫�? */
#define configTIMER_TASK_STACK_DEPTH                    ( configMINIMAL_STACK_SIZE * 2) /* 瀹氫箟锟??浠跺畾鏃跺櫒浠�?��?�鐨�?爤绌�?棿澶у�?, 鏃犻�?�???configUSE_TIMERS�???1鏃堕渶瀹氫�? */

/* �???�?夊嚱�???, 1: 浣胯�? */
#define INCLUDE_vTaskPrioritySet                        1                       /* 璁剧疆浠诲�?�浼樺厛�??? */
#define INCLUDE_uxTaskPriorityGet                       1                       /* 鑾峰彇浠诲�?�浼樺厛�??? */
#define INCLUDE_vTaskDelete                             1                       /* 鍒犻櫎浠诲�?? */
#define INCLUDE_vTaskSuspend                            1                       /* 鎸傝捣浠诲�?? */
#define INCLUDE_xResumeFromISR                          1                       /* �??�???锟藉�?�???�???�???鎸傝捣鐨�?换锟?? */
#define INCLUDE_vTaskDelayUntil                         1                       /* 浠�?��?�缁濓拷?锟藉欢锟?? */
#define INCLUDE_vTaskDelay                              1                       /* 浠�?��?��?�舵�? */
#define INCLUDE_xTaskGetSchedulerState                  1                       /* 鑾峰彇浠诲�?�璋冨�?�鍣ㄧ姸�??? */
#define INCLUDE_xTaskGetCurrentTaskHandle               1                       /* 鑾峰彇褰撳�?�浠诲�?�鐨�?换鍔″彞�??? */
#define INCLUDE_uxTaskGetStackHighWaterMark             1                       /* 鑾峰彇浠诲�?�鍫嗘爤鍘嗗彶鍓╀綑鏈�?灏忥�?? */
#define INCLUDE_xTaskGetIdleTaskHandle                  1                       /* 鑾峰彇绌�?棽浠诲�?�鐨�?换鍔″彞�??? */
#define INCLUDE_eTaskGetState                           1                       /* 鑾峰彇浠诲�?�鐘讹拷? */
#define INCLUDE_xEventGroupSetBitFromISR                1                       /* 鍦ㄤ腑锟??�???璁剧疆浜嬩�?�鏍囧織�??? */
#define INCLUDE_xTimerPendFunctionCall                  1                       /* 灏嗗嚱鏁�?殑鎵э�??锟芥寕鍒板畾鏃跺櫒鏈嶅�?�浠诲�?? */
#define INCLUDE_xTaskAbortDelay                         1                       /* �???�???浠�?��?��?�舵�? */
#define INCLUDE_xTaskGetHandle                          1                       /* �?氳繃浠�?��?�鍚嶈幏鍙栦换鍔″彞�??? */
#define INCLUDE_xTaskResumeFromISR                      1                       /* �??�???锟藉�?�???�???�???鎸傝捣鐨�?换锟?? */

/* �???�???宓岋�??锟斤�??锟戒负閰嶇疆 */
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS 4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15                  /* �???�???鏈€浣庝�?鍏堢�? */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5                   /* FreeRTOS�???绠＄悊鐨�?渶�?�樹腑锟??浼樺厛锟?? */
#define configKERNEL_INTERRUPT_PRIORITY                 ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_API_CALL_INTERRUPT_PRIORITY           configMAX_SYSCALL_INTERRUPT_PRIORITY

/* FreeRTOS�???�???鏈嶅姟鍑芥暟鐩�?�叧瀹氫�? */
#define xPortPendSVHandler                              PendSV_Handler
#define vPortSVCHandler                                 SVC_Handler

/* �???瑷€ */
#define vAssertCalled(char, int) printf("Error: %s, %d\r\n", char, int)
#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )

//flash根目�?
#define ffconfigDEV_PATH "/"
//在大多数情况下， FAT 表在磁盘上有两个相同的副�?�? �?以在读取错�??的情况下使用�?二个�?�?�? 如果�?
//设置�? 1，即�?使用两个 FAT，效率较低，但更 安全�?
//设置�? 0，即�?仅使用一�? FAT，�??二个 FAT 将永远不会�??写入�?
#define ffconfigWRITE_BOTH_FATS    1
//设置�? 1，即�?维护文件和目录时间戳以进行创建、修�? 和最后�?�问�?
//设置�? 0，即�?排除 时间戳�?
//如果使用时间�?持，则必须提供以下函数：
//time_t FreeRTOS_time( time_t *pxTime );	
//与标�? time() 函数�?义相同的 FreeRTOS_time�?
#define ffconfigTIME_SUPPORT    0
//设置�? 1，即�?维护当前各项任务的工作目�? (CWD)�? 该目录可访问文件系统，允许使用相对路径�?
//设置�? 0，即�?禁用 CWD，在这�?�情况下，必须使用完整路�? 访问每个文件�?
//#define ffconfigHAS_CWD    1
//#define ffconfigMKDIR_RECURSIVE 1 
#define FF_PRINTF    printf
#define ffconfigLFN_SUPPORT 1


/* FreeRTOS MPU 鐗�?�畩瀹氫�? */
//#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0
//#define configTOTAL_MPU_REGIONS                                8
//#define configTEX_S_C_B_FLASH                                  0x07UL
//#define configTEX_S_C_B_SRAM                                   0x07UL
//#define configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY            1
//#define configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS             1

/* ARMv8-M 瀹�?�叏渚э拷??鍙ｇ浉鍏冲畾涔�?�拷? */
//#define secureconfigMAX_SECURE_CONTEXTS         5

#endif /* FREERTOS_CONFIG_H */
