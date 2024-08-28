
#ifndef LWIP_HDR_APPS_LWIPERF_H
#define LWIP_HDR_APPS_LWIPERF_H

#include "lwip/opt.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LWIPERF_TCP_PORT_DEFAULT  5001

/** lwIPerf test results */
enum lwiperf_report_type
{
  /** The server side test is done */
  LWIPERF_TCP_DONE_SERVER,
  /** The client side test is done */
  LWIPERF_TCP_DONE_CLIENT,
  /** Local error lead to test abort */
  LWIPERF_TCP_ABORTED_LOCAL,
  /** Data check error lead to test abort */
  LWIPERF_TCP_ABORTED_LOCAL_DATAERROR,
  /** Transmit error lead to test abort */
  LWIPERF_TCP_ABORTED_LOCAL_TXERROR,
  /** Remote side aborted the test */
  LWIPERF_TCP_ABORTED_REMOTE
};

/** Control */
enum lwiperf_client_type
{
  /** Unidirectional tx only test */
  LWIPERF_CLIENT,
  /** Do a bidirectional test simultaneously */
  LWIPERF_DUAL,
  /** Do a bidirectional test individually */
  LWIPERF_TRADEOFF
};

/** Prototype of a report function that is called when a session is finished.
    This report function can show the test results.
    @param report_type contains the test result */
typedef void (*lwiperf_report_fn)(void *arg, enum lwiperf_report_type report_type,
  const ip_addr_t* local_addr, u16_t local_port, const ip_addr_t* remote_addr, u16_t remote_port,
  u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec);

void* lwiperf_start_tcp_server(const ip_addr_t* local_addr, u16_t local_port,
                               lwiperf_report_fn report_fn, void* report_arg);
void* lwiperf_start_tcp_server_default(lwiperf_report_fn report_fn, void* report_arg);
void* lwiperf_start_tcp_client(const ip_addr_t* remote_addr, u16_t remote_port,
                               enum lwiperf_client_type type,
                               lwiperf_report_fn report_fn, void* report_arg);
void* lwiperf_start_tcp_client_default(const ip_addr_t* remote_addr,
                               lwiperf_report_fn report_fn, void* report_arg);

void  lwiperf_abort(void* lwiperf_session);


#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_LWIPERF_H */
