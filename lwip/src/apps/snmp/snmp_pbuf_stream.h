

#ifndef LWIP_HDR_APPS_SNMP_PBUF_STREAM_H
#define LWIP_HDR_APPS_SNMP_PBUF_STREAM_H

#include "lwip/apps/snmp_opts.h"

#if LWIP_SNMP

#include "lwip/err.h"
#include "lwip/pbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

struct snmp_pbuf_stream {
  struct pbuf *pbuf;
  u16_t offset;
  u16_t length;
};

err_t snmp_pbuf_stream_init(struct snmp_pbuf_stream *pbuf_stream, struct pbuf *p, u16_t offset, u16_t length);
err_t snmp_pbuf_stream_read(struct snmp_pbuf_stream *pbuf_stream, u8_t *data);
err_t snmp_pbuf_stream_write(struct snmp_pbuf_stream *pbuf_stream, u8_t data);
err_t snmp_pbuf_stream_writebuf(struct snmp_pbuf_stream *pbuf_stream, const void *buf, u16_t buf_len);
err_t snmp_pbuf_stream_writeto(struct snmp_pbuf_stream *pbuf_stream, struct snmp_pbuf_stream *target_pbuf_stream, u16_t len);
err_t snmp_pbuf_stream_seek(struct snmp_pbuf_stream *pbuf_stream, s32_t offset);
err_t snmp_pbuf_stream_seek_abs(struct snmp_pbuf_stream *pbuf_stream, u32_t offset);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_SNMP */

#endif /* LWIP_HDR_APPS_SNMP_PBUF_STREAM_H */
