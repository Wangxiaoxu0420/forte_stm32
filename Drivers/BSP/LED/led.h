
#ifndef __LED_H
#define __LED_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 引脚 定义 */

#define LED0_GPIO_PORT                  GPIOF
#define LED0_GPIO_PIN                   GPIO_PIN_9
#define LED0_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)             /* PF口时钟使能 */



/******************************************************************************************/

/* LED端口定义 */
#define LED0(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)       /* LED0 = GREEN */

/* LED取反定义 */
#define LED0_TOGGLE()    do{ HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN); }while(0)       /* LED0 = !LED0 */

/******************************************************************************************/

enum {
    IP_GETTING,
    MQTT_CONNECTING,
    MQTT_CONNECTED,
    MQTT_DATA_TRANSLATE,
    OTA_FILE_GETTING,
    OTA_UPGRADE,
    LED_OFF,
    LED_ON
};
/* 外部接口函数*/
void led_init(void);                                                                            /* 初始化 */


void led_blink(void);
void led_delay(uint8_t mode);
void set_led_mode(uint8_t mode);

#define LED_BLINK()    do{ led_blink(); }while(0)       /* LED0 = !LED0 */
#define LED_DELAY(mode)    do{ led_delay(mode); }while(0)       /* LED0 = !LED0 */
#define LED_MODE(mode)    do{ set_led_mode(mode); }while(0)       /* LED0 = !LED0 */

#endif
