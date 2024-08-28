
#ifndef LWIP_HDR_APPS_MQTT_OPTS_H
#define LWIP_HDR_APPS_MQTT_OPTS_H

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup mqtt_opts Options
 * @ingroup mqtt
 * @{
 */

/**
 * Output ring-buffer size, must be able to fit largest outgoing publish message topic+payloads
 */
#ifndef MQTT_OUTPUT_RINGBUF_SIZE
#define MQTT_OUTPUT_RINGBUF_SIZE 1024
#endif

/**
 * Number of bytes in receive buffer, must be at least the size of the longest incoming topic + 8
 * If one wants to avoid fragmented incoming publish, set length to max incoming topic length + max payload length + 8
 */
#ifndef MQTT_VAR_HEADER_BUFFER_LEN
/* start: wangxiaoxu 20230607 lable_002 */
/* reason: increase MQTT receive buf size to 2048. */
/**
 * local/sbdm/device_id/config  LEN = 23, exclude local, start with /sbdm/device_id/config, so length is 23
 * so max payload length is 128-23-8 = 97.
 * we can set it to a large number for receive more data.
*/
//#define MQTT_VAR_HEADER_BUFFER_LEN 128
#define MQTT_VAR_HEADER_BUFFER_LEN 2048
/* end: wangxiaoxu 20230607 lable_002 */
#endif

/**
 * Maximum number of pending subscribe, unsubscribe and publish requests to server .
 */
#ifndef MQTT_REQ_MAX_IN_FLIGHT
/* start: wangxiaoxu 20230605 lable_001 */
/* reason: modify MAX MQTT requests to 8, causeof PUBLISH&SUBSCRIBE dir is more than 4. So set it to 8. */
//#define MQTT_REQ_MAX_IN_FLIGHT 4
#define MQTT_REQ_MAX_IN_FLIGHT 8
/* end: wangxiaoxu 20230605 lable_001 */
#endif

/**
 * Seconds between each cyclic timer call.
 */
#ifndef MQTT_CYCLIC_TIMER_INTERVAL
#define MQTT_CYCLIC_TIMER_INTERVAL 5
#endif

/**
 * Publish, subscribe and unsubscribe request timeout in seconds.
 */
#ifndef MQTT_REQ_TIMEOUT
#define MQTT_REQ_TIMEOUT 30
#endif

/**
 * Seconds for MQTT connect response timeout after sending connect request
 */
#ifndef MQTT_CONNECT_TIMOUT
#define MQTT_CONNECT_TIMOUT 100
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_MQTT_OPTS_H */
