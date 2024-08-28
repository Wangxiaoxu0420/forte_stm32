

#ifndef LWIP_HDR_PROT_AUTOIP_H
#define LWIP_HDR_PROT_AUTOIP_H

#ifdef __cplusplus
extern "C" {
#endif

/* 169.254.0.0 */
#define AUTOIP_NET              0xA9FE0000
/* 169.254.1.0 */
#define AUTOIP_RANGE_START      (AUTOIP_NET | 0x0100)
/* 169.254.254.255 */
#define AUTOIP_RANGE_END        (AUTOIP_NET | 0xFEFF)

/* RFC 3927 Constants */
#define PROBE_WAIT              1   /* second   (initial random delay)                 */
#define PROBE_MIN               1   /* second   (minimum delay till repeated probe)    */
#define PROBE_MAX               2   /* seconds  (maximum delay till repeated probe)    */
#define PROBE_NUM               3   /*          (number of probe packets)              */
#define ANNOUNCE_NUM            2   /*          (number of announcement packets)       */
#define ANNOUNCE_INTERVAL       2   /* seconds  (time between announcement packets)    */
#define ANNOUNCE_WAIT           2   /* seconds  (delay before announcing)              */
#define MAX_CONFLICTS           10  /*          (max conflicts before rate limiting)   */
#define RATE_LIMIT_INTERVAL     60  /* seconds  (delay between successive attempts)    */
#define DEFEND_INTERVAL         10  /* seconds  (min. wait between defensive ARPs)     */

/* AutoIP client states */
typedef enum {
  AUTOIP_STATE_OFF        = 0,
  AUTOIP_STATE_PROBING    = 1,
  AUTOIP_STATE_ANNOUNCING = 2,
  AUTOIP_STATE_BOUND      = 3
} autoip_state_enum_t;

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_PROT_AUTOIP_H */
