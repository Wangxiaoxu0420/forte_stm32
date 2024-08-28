
 
#include "lwip_comm.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "ethernetif.h"
#include "lwip/timeouts.h"
#include "lwip/tcpip.h"
#include "./MALLOC/malloc.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "log.h"


__lwip_dev g_lwipdev;                   /* lwip控制结构体 */
struct netif g_lwip_netif;              /* 定义一个全局的网络接口 */

#if LWIP_DHCP
__IO uint8_t g_lwip_dhcp_state = LWIP_DHCP_OFF;         /* DHCP状态初始化 */
#endif

/* LINK线程配置 */
#define LWIP_LINK_TASK_PRIO             16                   /* 任务优先级 */
#define LWIP_LINK_STK_SIZE              512 * 2             /* 任务堆栈大小 */
void lwip_link_thread( void * argument );                   /* 链路线程 */

/* DHCP线程配置 */
#define LWIP_DHCP_TASK_PRIO             17                   /* 任务优先级 */
#define LWIP_DHCP_STK_SIZE              128 * 2             /* 任务堆栈大小 */
void lwip_periodic_handle(void *argument);                  /* DHCP线程 */
void lwip_link_status_updated(struct netif *netif);         /* DHCP状态回调函数 */

/**
 * @breif       lwip 默认IP设置
 * @param       lwipx  : lwip控制结构体指针
 * @retval      无
 */
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
    /* 默认远端IP为:192.168.1.134 */
    lwipx->remoteip[0] = 192;
    lwipx->remoteip[1] = 168;
    lwipx->remoteip[2] = 1;
    lwipx->remoteip[3] = 1;
    
    /* MAC地址设置(高三字节固定为:2.0.0,低三字节用STM32唯一ID) */
    lwipx->mac[0] = 0xB8;       /* 高三字节(IEEE称之为组织唯一ID,OUI)地址固定为:2.0.0 */
    lwipx->mac[1] = 0xAE;
    lwipx->mac[2] = 0x1D;
    lwipx->mac[3] = 0x00;       /* 低三字节用STM32的唯一ID 28.0.36 */   
    lwipx->mac[4] = 0x07;
    lwipx->mac[5] = 0x00;
    
    //MAC地址使用flash最后几个字节，出厂烧录进去，跟序列号一样
    //如果该地址没烧录，则值为擦除后的全FF，判断如果非FF则认为烧录了MAC地址，否则使用默认MAC地址

    lwipx->mac[0] = (COLLECTORID[12]-'0') << 4 | (COLLECTORID[13]-'0');       /* 高三字节(IEEE称之为组织唯一ID,OUI)地址固定为:2.0.0 */
    lwipx->mac[1] = (COLLECTORID[14]-'0') << 4 | (COLLECTORID[15]-'0');
    lwipx->mac[2] = (COLLECTORID[16]-'0') << 4 | (COLLECTORID[17]-'0');
    lwipx->mac[3] = (COLLECTORID[24]-'0') << 4 | (COLLECTORID[25]-'0');       /* 低三字节用STM32的唯一ID 28.0.36 */   
    lwipx->mac[4] = (COLLECTORID[26]-'0') << 4 | (COLLECTORID[27]-'0');
    lwipx->mac[5] = (COLLECTORID[28]-'0') << 4 | (COLLECTORID[29]-'0');

    /* 默认本地IP为:192.168.101.30 */
    lwipx->ip[0] = 192;
    lwipx->ip[1] = 168;
    lwipx->ip[2] = 1;
    lwipx->ip[3] = 122;
    /* 默认子网掩码:255.255.255.0 */
    lwipx->netmask[0] = 255;
    lwipx->netmask[1] = 255;
    lwipx->netmask[2] = 255;
    lwipx->netmask[3] = 0;
    
    /* 默认网关:192.168.1.1 */
    lwipx->gateway[0] = 192;
    lwipx->gateway[1] = 168;
    lwipx->gateway[2] = 1;
    lwipx->gateway[3] = 1;
    lwipx->dhcpstatus = 0; /* 没有DHCP */
}

/**
 * @breif       LWIP初始化(LWIP启动的时候使用)
 * @param       无
 * @retval      0,成功
 *              1,内存错误
 *              2,以太网芯片初始化失败
 *              3,网卡添加失败.
 */
uint8_t lwip_comm_init(void)
{
    uint8_t retry = 0;
    struct netif *netif_init_flag;              /* 调用netif_add()函数时的返回值,用于判断网络初始化是否成功 */
    ip_addr_t ipaddr;                           /* ip地址 */
    ip_addr_t netmask;                          /* 子网掩码 */
    ip_addr_t gw;                               /* 默认网关 */
    
    tcpip_init(NULL, NULL);
    
    if (ethernet_mem_malloc())return 1;         /* 内存申请失败*/

    lwip_comm_default_ip_set(&g_lwipdev);         /* 设置默认IP等信息 */

    while (ethernet_init())                     /* 初始化以太网芯片,如果失败的话就重试5次 */
    {
        retry++;

        if (retry > 5)
        {
            retry = 0;                          /* 以太网芯片初始化失败 */
            return 3;
        }
    }

#if LWIP_DHCP                                   /* 使用动态IP */
    ip_addr_set_zero_ip4(&ipaddr);              /* 对IP地址、子网掩码及网关清零 */
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);
#else   /* 使用静态IP */
    IP4_ADDR(&ipaddr, g_lwipdev.ip[0], g_lwipdev.ip[1], g_lwipdev.ip[2], g_lwipdev.ip[3]);
    IP4_ADDR(&netmask, g_lwipdev.netmask[0], g_lwipdev.netmask[1], g_lwipdev.netmask[2], g_lwipdev.netmask[3]);
    IP4_ADDR(&gw, g_lwipdev.gateway[0], g_lwipdev.gateway[1], g_lwipdev.gateway[2], g_lwipdev.gateway[3]);
    printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n", g_lwipdev.mac[0], g_lwipdev.mac[1], g_lwipdev.mac[2], g_lwipdev.mac[3], g_lwipdev.mac[4], g_lwipdev.mac[5]);
    printf("静态IP地址........................%d.%d.%d.%d\r\n", g_lwipdev.ip[0], g_lwipdev.ip[1], g_lwipdev.ip[2], g_lwipdev.ip[3]);
    printf("子网掩码..........................%d.%d.%d.%d\r\n", g_lwipdev.netmask[0], g_lwipdev.netmask[1], g_lwipdev.netmask[2], g_lwipdev.netmask[3]);
    printf("默认网关..........................%d.%d.%d.%d\r\n", g_lwipdev.gateway[0], g_lwipdev.gateway[1], g_lwipdev.gateway[2], g_lwipdev.gateway[3]);
#endif  /* 向网卡列表中添加一个网口 */
    netif_init_flag = netif_add(&g_lwip_netif, (const ip_addr_t *)&ipaddr, (const ip_addr_t *)&netmask, (const ip_addr_t *)&gw, NULL, &ethernetif_init, &tcpip_input);


    if (netif_init_flag == NULL)
    {
        return 4;                           /* 网卡添加失败 */
    }
    else                                    /* 网口添加成功后,设置netif为默认值,并且打开netif网口 */
    {
        netif_set_default(&g_lwip_netif);     /* 设置netif为默认网口 */

        if (netif_is_link_up(&g_lwip_netif))
        {
            netif_set_up(&g_lwip_netif);      /* 打开netif网口 */
        }
        else
        {
            netif_set_down(&g_lwip_netif);
        }
        
#if LWIP_NETIF_LINK_CALLBACK
        lwip_link_status_updated(&g_lwip_netif);    /* DHCP链接状态更新函数 */
        netif_set_link_callback(&g_lwip_netif, lwip_link_status_updated);
        /* 查询PHY连接状态任务 */
        sys_thread_new("eth_link",
                       lwip_link_thread,            /* 任务入口函数 */
                       &g_lwip_netif,               /* 任务入口函数参数 */
                       LWIP_LINK_STK_SIZE,          /* 任务栈大小 */
                       LWIP_LINK_TASK_PRIO);        /* 任务的优先级 */
#endif
    }
    
    g_lwipdev.link_status = LWIP_LINK_OFF;          /* 链接标记为0 */
#if LWIP_DHCP                                       /* 如果使用DHCP的话 */
    g_lwipdev.dhcpstatus = 0;                       /* DHCP标记为0 */
    /* DHCP轮询任务 */
    sys_thread_new("eth_dhcp",
                   lwip_periodic_handle,            /* 任务入口函数 */
                   &g_lwip_netif,                   /* 任务入口函数参数 */
                   LWIP_DHCP_STK_SIZE,              /* 任务栈大小 */
                   LWIP_DHCP_TASK_PRIO);            /* 任务的优先级 */
#endif
    return 0;                               /* 操作OK. */
}

/**
 * @brief       通知用户网络接口配置状态
 * @param       netif：网卡控制块
 * @retval      无
 */
void lwip_link_status_updated(struct netif *netif)
{
    if (netif_is_up(netif))
    {
#if LWIP_DHCP
        /* Update DHCP state machine */
        g_lwip_dhcp_state = LWIP_DHCP_START;
        printf ("The network cable is connected \r\n");
#endif /* LWIP_DHCP */
    }
    else
    {
#if LWIP_DHCP
        /* Update DHCP state machine */
        g_lwip_dhcp_state = LWIP_DHCP_LINK_DOWN;
        printf ("The network cable is not connected \r\n");
#endif /* LWIP_DHCP */
    }
}



extern xSemaphoreHandle g_rx_semaphore; /* 定义一个信号量 */
/**
 * @breif       当接收到数据后调用
 * @param       无
 * @retval      无
 */
void lwip_pkt_handle(void)
{
    BaseType_t xHigherPriorityTaskWoken;
    /* 获取信号量 */
    xSemaphoreGiveFromISR(g_rx_semaphore,&xHigherPriorityTaskWoken);/* 释放二值信号量 */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);                   /* 如果需要的话进行一次任务切换  */
}



/* 如果使能DHCP */
#if LWIP_DHCP

/**
 * @breif       DHCP进程
 * @param       argument:传入的形参
 * @retval      无
 */
void lwip_periodic_handle(void *argument)
{
     struct netif *netif = (struct netif *) argument;
    uint32_t ip = 0;
    uint32_t netmask = 0;
    uint32_t gw = 0;
    struct dhcp *dhcp;
    uint8_t iptxt[20];

    while (1)
    {
        switch (g_lwip_dhcp_state)
        {
            case LWIP_DHCP_START:
            {
                /* 对IP地址、网关地址及子网页码清零操作 */
                ip_addr_set_zero_ip4(&netif->ip_addr);
                ip_addr_set_zero_ip4(&netif->netmask);
                ip_addr_set_zero_ip4(&netif->gw);
                ip_addr_set_zero_ip4(&netif->ip_addr);
                ip_addr_set_zero_ip4(&netif->netmask);
                ip_addr_set_zero_ip4(&netif->gw);
                
                g_lwip_dhcp_state = LWIP_DHCP_WAIT_ADDRESS;
                
                printf ("State: Looking for DHCP server ...\r\n");
                dhcp_start(netif);
            }
            break;
            case LWIP_DHCP_WAIT_ADDRESS:
            {
                if (dhcp_supplied_address(netif))
                {
                    g_lwip_dhcp_state = LWIP_DHCP_ADDRESS_ASSIGNED;
                    
                    ip = g_lwip_netif.ip_addr.addr;       /* 读取新IP地址 */
                    netmask = g_lwip_netif.netmask.addr;  /* 读取子网掩码 */
                    gw = g_lwip_netif.gw.addr;            /* 读取默认网关 */
                    
                    sprintf((char *)iptxt, "%s", ip4addr_ntoa(netif_ip4_addr(netif)));
                    printf ("IP address assigned by a DHCP server: %s\r\n", iptxt);
                    
                    if (ip != 0)
                    {
                        g_lwipdev.dhcpstatus = 2;         /* DHCP成功 */
                        printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n", g_lwipdev.mac[0], g_lwipdev.mac[1], g_lwipdev.mac[2], g_lwipdev.mac[3], g_lwipdev.mac[4], g_lwipdev.mac[5]);
                        /* 解析出通过DHCP获取到的IP地址 */
                        g_lwipdev.ip[3] = (uint8_t)(ip >> 24);
                        g_lwipdev.ip[2] = (uint8_t)(ip >> 16);
                        g_lwipdev.ip[1] = (uint8_t)(ip >> 8);
                        g_lwipdev.ip[0] = (uint8_t)(ip);
                        printf("通过DHCP获取到IP地址..............%d.%d.%d.%d\r\n", g_lwipdev.ip[0], g_lwipdev.ip[1], g_lwipdev.ip[2], g_lwipdev.ip[3]);
                        /* 解析通过DHCP获取到的子网掩码地址 */
                        g_lwipdev.netmask[3] = (uint8_t)(netmask >> 24);
                        g_lwipdev.netmask[2] = (uint8_t)(netmask >> 16);
                        g_lwipdev.netmask[1] = (uint8_t)(netmask >> 8);
                        g_lwipdev.netmask[0] = (uint8_t)(netmask);
                        printf("通过DHCP获取到子网掩码............%d.%d.%d.%d\r\n", g_lwipdev.netmask[0], g_lwipdev.netmask[1], g_lwipdev.netmask[2], g_lwipdev.netmask[3]);
                        /* 解析出通过DHCP获取到的默认网关 */
                        g_lwipdev.gateway[3] = (uint8_t)(gw >> 24);
                        g_lwipdev.gateway[2] = (uint8_t)(gw >> 16);
                        g_lwipdev.gateway[1] = (uint8_t)(gw >> 8);
                        g_lwipdev.gateway[0] = (uint8_t)(gw);
                        printf("通过DHCP获取到的默认网关..........%d.%d.%d.%d\r\n", g_lwipdev.gateway[0], g_lwipdev.gateway[1], g_lwipdev.gateway[2], g_lwipdev.gateway[3]);
                        
                        g_lwipdev.lwip_display_fn(2);
                    }
                }
                else
                {
                    dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

                    /* DHCP timeout */
                    if (dhcp->tries > LWIP_MAX_DHCP_TRIES)
                    {
                        g_lwip_dhcp_state = LWIP_DHCP_TIMEOUT;
                        g_lwipdev.dhcpstatus = 0XFF;
                        /* 使用静态IP地址 */
                        IP4_ADDR(&(g_lwip_netif.ip_addr), g_lwipdev.ip[0], g_lwipdev.ip[1], g_lwipdev.ip[2], g_lwipdev.ip[3]);
                        IP4_ADDR(&(g_lwip_netif.netmask), g_lwipdev.netmask[0], g_lwipdev.netmask[1], g_lwipdev.netmask[2], g_lwipdev.netmask[3]);
                        IP4_ADDR(&(g_lwip_netif.gw), g_lwipdev.gateway[0], g_lwipdev.gateway[1], g_lwipdev.gateway[2], g_lwipdev.gateway[3]);
                        netif_set_addr(netif, &g_lwip_netif.ip_addr, &g_lwip_netif.netmask, &g_lwip_netif.gw);

                        sprintf((char *)iptxt, "%s", ip4addr_ntoa(netif_ip4_addr(netif)));
                        printf ("DHCP Timeout !! \r\n");
                        printf ("Static IP address: %s\r\n", iptxt);
                        g_lwipdev.lwip_display_fn(2);
                    }
                }
            }
            break;
            case LWIP_DHCP_LINK_DOWN:
            {
                g_lwip_dhcp_state = LWIP_DHCP_OFF;
            }
            break;
            default: break;
        }

        /* wait 1000 ms */
        vTaskDelay(1000);
    }
}
#endif

#if LWIP_NETIF_LINK_CALLBACK
/**
  * @brief       检查ETH链路状态，更新netif
  * @param       argument: netif
  * @retval      无
  */
void lwip_link_thread( void * argument )
{
    uint32_t regval = 0;
    struct netif *netif = (struct netif *) argument;
    int link_again_num = 0;

    while(1)
    {
        /* 读取PHY状态寄存器，获取链接信息 */
        HAL_ETH_ReadPHYRegister(&g_eth_handler,PHY_BSR, &regval);

        /* 判断链接状态 */
        if((regval & PHY_LINKED_STATUS) == 0)
        {
            //printf("netif_set_link_down\r\n");
            g_lwipdev.link_status = LWIP_LINK_OFF;
            
            link_again_num ++ ;
            
            if (link_again_num >= 2)                    /* 网线一段时间没有插入 */
            {
                vTaskDelay(100);
                continue;
            }
            else                                        /* 关闭虚拟网卡及以太网中断 */
            {
                LOG_E("The network cable has been unplugged!!!");
                //LOG_E("The network cable has been unplugged!!!");
#if LWIP_DHCP                                           /* 如果使用DHCP的话 */
                g_lwip_dhcp_state = LWIP_DHCP_LINK_DOWN;

                dhcp_stop(netif);
#endif
                HAL_ETH_Stop(&g_eth_handler);
                netif_set_down(netif);
                netif_set_link_down(netif);
            }
        }
        else                                            /* 网线插入检测 */
        {
            link_again_num = 0;

            if (g_lwipdev.link_status == LWIP_LINK_OFF)/* 开启以太网及虚拟网卡 */
            {
                LOG_E("LWIP_LINK_ON");
                //LOG_E("LWIP_LINK_ON");
                g_lwipdev.link_status = LWIP_LINK_ON;
                HAL_ETH_Start(&g_eth_handler);
                netif_set_up(netif);
                netif_set_link_up(netif);
            }
        }

        vTaskDelay(100);
    }
}
#endif
