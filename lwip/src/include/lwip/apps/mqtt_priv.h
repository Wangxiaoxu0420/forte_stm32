
#ifndef LWIP_HDR_APPS_MQTT_PRIV_H
#define LWIP_HDR_APPS_MQTT_PRIV_H

#include "lwip/apps/mqtt.h"
#include "lwip/altcp.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Pending request item, binds application callback to pending server requests */
struct mqtt_request_t
{
  /** Next item in list, NULL means this is the last in chain,
      next pointing at itself means request is unallocated */
  struct mqtt_request_t *next;
  /** Callback to upper layer */
  mqtt_request_cb_t cb;
  void *arg;
  /** MQTT packet identifier */
  u16_t pkt_id;
  /** Expire time relative to element before this  */
  u16_t timeout_diff;
};

/** Ring buffer */
struct mqtt_ringbuf_t {
  u16_t put;
  u16_t get;
  u8_t buf[MQTT_OUTPUT_RINGBUF_SIZE];
};

/** MQTT client */
struct mqtt_client_s
{
  /** Timers and timeouts */
  u16_t cyclic_tick;
  u16_t keep_alive;
  u16_t server_watchdog;
  /** Packet identifier generator*/
  u16_t pkt_id_seq;
  /** Packet identifier of pending incoming publish */
  u16_t inpub_pkt_id;
  /** Connection state */
  u8_t conn_state;
  struct altcp_pcb *conn;
  /** Connection callback */
  void *connect_arg;
  mqtt_connection_cb_t connect_cb;
  /** Pending requests to server */
  struct mqtt_request_t *pend_req_queue;
  struct mqtt_request_t req_list[MQTT_REQ_MAX_IN_FLIGHT];
  void *inpub_arg;
  /** Incoming data callback */
  mqtt_incoming_data_cb_t data_cb;
  mqtt_incoming_publish_cb_t pub_cb;
  /** Input */
  u32_t msg_idx;
  u8_t rx_buffer[MQTT_VAR_HEADER_BUFFER_LEN];
  /** Output ring-buffer */
  struct mqtt_ringbuf_t output;
};

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_MQTT_PRIV_H */
