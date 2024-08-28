
 
 #include "./BSP/LED/led.h"

/**
 * @brief       初始化LED相关IO口, 并使能时钟
 * @param       无
 * @retval      无
 */
void led_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    LED0_GPIO_CLK_ENABLE();                                 /* LED0时钟使能 */

    gpio_init_struct.Pin = LED0_GPIO_PIN;                   /* LED0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct);       /* 初始化LED0引脚 */
    
    LED0(1);                                                /* 关闭 LED0 */
}
static uint16_t on_ms,off_ms,count,mode;
void led_blink(void)
{
    for (size_t i = 0; i < count; i++)
    {
        if(on_ms)
        {
            LED0(0);
            vTaskDelay(on_ms);
        }

        if (off_ms)
        {
            LED0(1);
            vTaskDelay(off_ms);
        }
    }
    if(mode == MQTT_DATA_TRANSLATE )
    {
        count = 0;//不闪烁
    }
}
void led_delay(uint8_t mode)
{
    uint16_t set_on_ms,set_off_ms;
    switch (mode)
    {
    case MQTT_DATA_TRANSLATE:
        set_on_ms   = 200;
        set_off_ms  = 200;
        break;
    default:
        break;
    }
    for (size_t i = 0; i < count; i++)
    {
        if(set_on_ms)
        {
            LED0(0);
            vTaskDelay(set_on_ms);
        }

        if (set_off_ms)
        {
            LED0(1);
            vTaskDelay(set_off_ms);
        }
    }
}

void set_led_mode(uint8_t set_mode)
{
    uint16_t set_on_ms,set_off_ms,set_count;
    switch (set_mode)
    {
    case IP_GETTING:
        set_on_ms   = 300;
        set_off_ms  = 2000;
        set_count = 1;
        break;
    case MQTT_CONNECTING:
        set_on_ms   = 300;
        set_off_ms  = 1000;
        set_count = 1;
        break;
    case MQTT_CONNECTED:
        set_on_ms   = 0;
        set_off_ms  = 1;
        set_count = 1;
        break;
    case MQTT_DATA_TRANSLATE:
        set_on_ms   = 100;
        set_off_ms  = 100;
        set_count = 2;
        break;
    case OTA_FILE_GETTING:
        set_on_ms   = 1;
        set_off_ms  = 0;
        set_count = 1;
        break;
    case LED_ON:
        set_on_ms   = 1000;
        set_off_ms  = 0;
        set_count = 1;
        break;
    case LED_OFF:
        set_on_ms   = 0;
        set_off_ms  = 1000;
        set_count = 1;
        break;
    
    default:
        break;
    }
    on_ms = set_on_ms;
    off_ms = set_off_ms;
    count = set_count;
    mode = set_mode;
}