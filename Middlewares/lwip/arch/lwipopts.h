#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/**
SYS_LIGHTWEIGHT_PROT==1:如果您确实需要任务间保护
 */
#define SYS_LIGHTWEIGHT_PROT            1

/* NO_SYS 表示无操作系统模拟层，无操作系统为1，有操作系统设置为0 
   注意这个参数设置会编译不同 */
#define NO_SYS                          0

/**
 * NO_SYS_NO_TIMERS==1: Drop support for sys_timeout when NO_SYS==1
 * Mainly for compatibility to old versions.
 */
#define NO_SYS_NO_TIMERS                0

/* ---------- 内存选项 ---------- */
/* 内存对齐，按照 4 字节对齐  */
#define MEM_ALIGNMENT                   4

/* 堆内存的大小，如果需要更大的堆内存，那么设置高一点 */
#define MEM_SIZE                        (29*1024)

/* MEMP_NUM_PBUF: 设置内存池的数量  */
#define MEMP_NUM_PBUF                   15
/* MEMP_NUM_UDP_PCB: UDP协议控制块的数量. */
#define MEMP_NUM_UDP_PCB                4
/* MEMP_NUM_TCP_PCB: TCP的数量. */
#define MEMP_NUM_TCP_PCB                4
/* MEMP_NUM_TCP_PCB_LISTEN: 监听TCP的数量. */
#define MEMP_NUM_TCP_PCB_LISTEN         2
/* MEMP_NUM_TCP_SEG: 同时排队的TCP的数量段. */
#define MEMP_NUM_TCP_SEG                120
/* MEMP_NUM_SYS_TIMEOUT: 超时模拟活动的数量. */
#define MEMP_NUM_SYS_TIMEOUT            8


/* ---------- Pbuf选项 ---------- */
/* PBUF_POOL 内存池中每个内存块大小 */
#define PBUF_POOL_SIZE                  20
/* PBUF_POOL_BUFSIZE: pbuf池中每个pbuf的大小. */
#define PBUF_POOL_BUFSIZE               LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_ENCAPSULATION_HLEN+PBUF_LINK_HLEN)


/* ---------- TCP选项 ---------- */
#define LWIP_TCP                        1
#define TCP_TTL                         255

/* 控制TCP是否应该对到达的段进行排队
   秩序。如果你的设备内存不足，定义为0. */
#define TCP_QUEUE_OOSEQ                 0

/* TCP最大段大小 */
#define TCP_MSS                         (1500 - 40)   /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

/* TCP发送者缓冲区空间(字节). */
#define TCP_SND_BUF                     (11*TCP_MSS)

/*  TCP_SND_QUEUELEN: TCP发送缓冲区空间。这必须是至少
    需要(2 * TCP_SND_BUF/TCP_MSS)才能正常工作 */

#define TCP_SND_QUEUELEN                (8* TCP_SND_BUF/TCP_MSS)

/* TCP接收窗口 */
#define TCP_WND                         (2*TCP_MSS)


/* ---------- ICMP 选项 ---------- */
#define LWIP_ICMP                       1

/* ---------- DNS 选项 ---------- */
#define LWIP_DNS                        1
#include <stdlib.h>
#define LWIP_RAND                       rand


/* ---------- DHCP 选项 ---------- */
/* 如果您希望DHCP配置为，请将LWIP_DHCP定义为1 */
#define LWIP_DHCP                       1


/* ---------- UDP 选项 ---------- */
#define LWIP_UDP                        1
#define UDP_TTL                         255

/* start: wangxiaoxu 20230605 lable_001 */
/* reason: add IGMP define to open UDP group */
/* ---------- IGMP 选项 ---------- */
#define LWIP_IGMP                       1
/* end: wangxiaoxu 20230605 lable_001 */

/* ---------- Statistics 选项 ---------- */
#define LWIP_STATS                      0
#define LWIP_PROVIDE_ERRNO              1

/* ---------- 链接回调选项 ---------- */
/* WIP_NETIF_LINK_CALLBACK==1:支持来自接口的回调函数
   每当链接改变(例如，向下链接)
 */
#define LWIP_NETIF_LINK_CALLBACK        1
/*
   --------------------------------------
   ---------- 帧校验和选项 ----------
   --------------------------------------
*/

/* 
The STM32F4x7 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature let the following define uncommented.
 - To disable it and process by CPU comment the  the checksum.
*/
#define CHECKSUM_BY_HARDWARE 


#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0 
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
  /* CHECKSUM_CHECK_ICMP==0: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
  /* CHECKSUM_CHECK_ICMP==1: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               1
#endif


/*
   ----------------------------------------------
   ---------- 连续层的选择 ----------
   ----------------------------------------------
*/
/**
 * LWIP_NETCONN==1:启用Netconn API(需要使用api_lib.c)
 */
#define LWIP_NETCONN                    1

/*
   ------------------------------------
   ---------- Socket选项 ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1:启用Socket API(要求使用Socket .c)
 */
#define LWIP_SOCKET                     1

/*
   ---------------------------------
   ---------- 操作系统选项 ----------
   ---------------------------------
*/


#define DEFAULT_UDP_RECVMBOX_SIZE       10
#define DEFAULT_TCP_RECVMBOX_SIZE       10
#define DEFAULT_ACCEPTMBOX_SIZE         10
#define DEFAULT_THREAD_STACKSIZE        1024


#define TCPIP_THREAD_NAME              "t_lwip_core"
#define TCPIP_THREAD_STACKSIZE          2048
#define TCPIP_MBOX_SIZE                 8
#define TCPIP_THREAD_PRIO               15
#define LWIP_SO_RCVTIMEO                1


/*
   ----------------------------------------
   ---------- Lwip调试选项 ----------
   ----------------------------------------
*/
#define LWIP_DEBUG                      1            /* 关闭DEBUG选项 */
#if LWIP_DEBUG
#define LWIP_DEBUG 1
#else
#undef LWIP_DEBUG
#endif

#define ICMP_DEBUG                      LWIP_DBG_OFF /* 开启/关闭ICMPdebug  */

#endif /* __LWIPOPTS_H__ */
