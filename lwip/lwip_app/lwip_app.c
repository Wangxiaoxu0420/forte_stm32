
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <netdb.h>
#include "lwip/apps/mqtt.h"
#include "./MALLOC/malloc.h"
#include "./BSP/LED/led.h"
#include "lwip_app.h"
#include "hmac.h"
#include "string.h"
#include "cJSON.h"
#include "sys.h"
#include <time.h>
#include "lwip_comm.h"

#include "lwip/api.h"
#include "lwip/tcp.h"

#include "./BSP/NORFLASH/norflash.h"
#include "md5.h"
#include "modbus.h"
#include "fs.h"
#include "freertos_task.h"
#include "rtc.h"
#include "./BSP/LED/led.h"

sys_thread_t t_load_conf_handler;
sys_thread_t t_publish_handler;
sys_thread_t t_upgrade_handler;
sys_thread_t t_load_conf_handler;
sys_thread_t t_publish_handler;
sys_thread_t t_udp_handler;
sys_thread_t t_tcp_handler;




/* start: wangxiaoxu 20230605 lable_001 */
/* reason: dynamics modify MQTT IP and port */
struct ip_port_t
{
    ip_addr_t ip_addr;
    u16_t port;
};

struct ip_port_t g_mqtt_ip_port;
struct ip_port_t g_xtbox_ip_port;
/* end: wangxiaoxu 20230605 lable_001 */

static mqtt_client_t* g_mqtt_client = NULL;
float g_temp = 0;    /* 温度值 */
float g_humid = 0;   /* 湿度值 */
unsigned char g_payload_out[200];
int g_payload_out_len = 0;
int g_publish_flag = 0;/* 发布成功标志位 */
int g_err_flag = 0;/* 故障标志位 */
int g_version_flag = 0;/* 版本标志位 */

int g_polling_record = 0;/* 补报标志位 */
uint64_t g_report_timestamp = 0;/* 上报数据的时间 */


/* reason: add aotu discovery. */
/* 多播 IP 地址 */
#define GROUP_IP "238.0.0.6"
#define GROUP_PORT 2020

#define DECLARE_IP "238.0.0.5"
#define DECLARE_PORT 2021

/* socket信息 */
struct link_socjet_info
{
    struct sockaddr_in client_addr; /* 网络地址信息 */
    socklen_t client_addr_len;      /* 网络地址信息长度 */
    int optval;                     /* 为存放选项值 */
    int sfd;                        /* socket控制块 */
    ip_mreq multicast_mreq;         /* 组播控制块 */
    
    struct
    {
        char *buf;               /* 缓冲空间 */
        uint32_t size;              /* 缓冲空间大小 */
    } send;                         /* 发送缓冲 */
    
    struct
    {
        char *buf;               /* 缓冲空间 */
        uint32_t size;              /* 缓冲空间大小 */
    } recv;                         /* 接收缓冲 */
};

/* 多播信息 */
struct ip_mreq_t
{
    struct ip_mreq mreq;            /* 多播信息控制块 */
    socklen_t mreq_len;             /* 多播信息长度 */
};
/* 接收数据缓冲区 */
static char g_lwip_demo_recvbuf[1024];
//发送BUF可以动态申请，发送完成之后，释放掉
//static uint8_t g_lwip_demo_sendbuf[256]; 
#define LWIP_SEND_BUF_SIZE 256

/* 获取一个全局的网络接口，用于修改MQTT服务器地址和端口 */
extern struct netif g_lwip_netif;              


#if 0
//online消息结构体，XTBOX和设备的online结构一样
struct online_message_t{
    char *cmd;
    uint32_t Timestamp;
    uint8_t Encryption;
    char IP[16];
    uint16_t port;
};

//设备上线消息的组帧
const struct online_message_t online_message = 
{
    "dev_online",
    0,
    0,
    "192.168.1.221",
    2020
};

//online ack消息应答结构体
struct online_ack_message_t{
    char *cmd;
    uint32_t Timestamp;
    uint8_t Encryption;
    char IP[16];
};

//设备应答消息组帧
const struct online_ack_message_t online_ack_message = 
{
    "online_ack",
    0,
    0,
    "192.168.1.221"
};



//设备回复网络配置的应答结构体
struct net_config_ack_message_t{
    char *cmd;
    uint32_t Timestamp;
    uint8_t Result;
};

//设备回复网络配置的组帧
const struct net_config_ack_message_t net_config_ack_message = 
{
    "net_conf_ack",
    0,
    0
};
//XTBOX设备下发网络配置的结构体
struct net_config_message_t{
    char *cmd;
    uint32_t Timestamp;
    char *SRCID;
    char *DESID;
    uint8_t Encryption;
    char IP[16];
    uint16_t port;
};
#endif


//后续参数要放到flash中，上电后读取出来
uint64_t g_heart_beat_time = 5; 
int g_heart_beat_len = 0;
/* end: wangxiaoxu 20230607 lable_002 */

/* start: wangxiaoxu 20230605 lable_001 */
/* reason: add MQTT net config complete flag. */
#define NET_CONFIG_NONE 0
#define NET_CONFIG_START 1
#define NET_CONFIG_OK 2

int g_MQTT_netconfig_complete_flag = NET_CONFIG_NONE;/* MQTT配置成功标志位 */
/* end: wangxiaoxu 20230605 lable_001 */

void lwip_ali_get_password(const char *device_secret, const char *content, char *password);


uint8_t *_LwipHeartBeatPacket(void)
{
    cJSON * json_head;
    cJSON * json_item;
    cJSON * json_term;
    char *pack;
    json_head = cJSON_CreateObject();

    cJSON_AddStringToObject(json_head,STR_CODE,             STR_REPORT_LINK_STATE);
    cJSON_AddNumberToObject(json_head,STR_SRC_TYPE,         0);
    cJSON_AddStringToObject(json_head,STR_DEV_ID,           COLLECTORID);
    cJSON_AddNumberToObject(json_head,STR_TIMESTAMP,        GetTimeStamp());
    cJSON_AddNumberToObject(json_head,STR_ENCRYPT,          0);
    cJSON_AddObjectToObject(json_head, STR_CONTENT);
    //如果MQTT服务器断连超过1小时，则需要重新发送版本信息
    #if 0
    (g_version_flag*5 >= 3600)
    {
        g_version_flag = 0;
        uint8_t buf[10];
        f_version(buf, READ|HARDWARE_VERSION);
        cJSON_AddStringToObject(json_head,STR_HARDWARE_VERSION,           buf);
        f_version(buf, READ|SOFTWARE_VERSION);
        cJSON_AddStringToObject(json_head,STR_SOFTWARE_VERSION,           buf);
        f_version(buf, READ|CONFIG_VERSION);
        cJSON_AddStringToObject(json_head,STR_DATA_VERSION,               buf);
    }
    #endif

    json_item = cJSON_GetObjectItem(json_head, STR_CONTENT);

    cJSON_AddStringToObject(json_item,STR_TERM_ID, "");
#if 0
    cJSON_AddArrayToObject(json_item,STR_TERM_ID);
	json_term = cJSON_GetObjectItem(json_item,STR_TERM_ID);
    cJSON_AddItemToArray(json_term,  cJSON_CreateString(COLLECTORID));
#endif
    cJSON_AddArrayToObject(json_item,STR_FAULT);
    cJSON_AddStringToObject(json_item,STR_SOFTWARE_VERSION,           VERSION_SOFTWARE);
    cJSON_AddStringToObject(json_item,STR_HARDWARE_VERSION,           VERSION_HARDWARE);
    
    json_item = cJSON_GetObjectItem(json_item,STR_FAULT);
    cJSON_AddItemToArray(json_item, cJSON_CreateNumber(0));


    pack = cJSON_PrintUnformatted(json_head);
    //if(pack){
    //    cJSON_free(pack);
    //}
    cJSON_Delete(json_head);
    return (uint8_t *)pack;
}



//组帧升级请求的Json包
uint8_t _LwipFilePacket(uint8_t err, uint8_t *updatebuf, uint8_t *cmd, uint8_t *filepath, uint8_t *filename, uint8_t *filetype, uint8_t *password )
{
    cJSON * json_update;
    char *pack;
    char *endchar = "\r\n";
    json_update = cJSON_CreateObject();

    //如果传入有cmd，则附带上该命令
    if(cmd)
    {
        cJSON_AddStringToObject(json_update,STR_CODE,           (char *)cmd);
    }
    cJSON_AddStringToObject(json_update,STR_DEV_ID,            COLLECTORID);
    cJSON_AddStringToObject(json_update,STR_FILE_PATH,         (char *)filepath);
    cJSON_AddStringToObject(json_update,STR_FILE_NAME,         (char *)filename);
    cJSON_AddStringToObject(json_update,STR_FILE_TYPE,         (char *)filetype);
    cJSON_AddStringToObject(json_update,STR_PASSWORD,          (char *)password);


    pack = cJSON_PrintUnformatted(json_update);
    printf("The JSON content:%s%s",pack,endchar);
    strcpy((char *)updatebuf, pack);
    if(pack){
        cJSON_free(pack);
    }
    cJSON_Delete(json_update);
    return 0;
}

//组帧升级请求的Json包
uint8_t _LwipFileAckPacket(uint8_t err, uint8_t *updatebuf, uint8_t *cmd)
{
    cJSON * json_update;
    char *pack;
    char *endchar = "\r\n";
    json_update = cJSON_CreateObject();

    //如果传入有cmd，则附带上该命令
    if(cmd)
    {
        cJSON_AddStringToObject(json_update,STR_CODE,           (char *)cmd);
    }

    if(err)
    {
        cJSON_AddStringToObject(json_update,STR_RESPONSE,           "ERROR");
    }
    else{
        cJSON_AddStringToObject(json_update,STR_RESPONSE,           "OK");
    }
    cJSON_AddNumberToObject(json_update,STR_ERROR_CODE,          err);

    pack = cJSON_PrintUnformatted(json_update);
    printf("The JSON content:%s%s",pack,endchar);
    strcpy((char *)updatebuf, pack);
    if(pack){
        cJSON_free(pack);
    }
    cJSON_Delete(json_update);
    return 0;
}

//组帧升级请求的Json包
uint8_t _LwipUpdatePacket(cJSON *json_head, uint8_t *updatebuf, uint8_t *upgrade_type)
{
    cJSON * json_update;
    cJSON * json_item;
    char *pack;
    char *endchar = "\r\n";
    json_update = cJSON_CreateObject();

    //if(!strcmp((char *)upgrade_type, STR_OTA_FILE_UPGRADE))   cJSON_AddStringToObject(json_update,STR_CODE,           STR_DEV_UPGRADE_REQ);
    if(!strcmp((char *)upgrade_type, STR_OTA_FILE_INFO))      cJSON_AddStringToObject(json_update,STR_CODE,           STR_DEV_CONFIG_REQ);

    json_item = cJSON_GetObjectItem(json_head,STR_DEV_ID);
    cJSON_AddStringToObject(json_update,STR_DEV_ID,           json_item->valuestring);

    json_item = cJSON_GetObjectItem(json_head,STR_CONTENT);
    json_item = cJSON_GetObjectItem(json_item,STR_FILE_PATH);
    cJSON_AddStringToObject(json_update,STR_FILE_PATH,           json_item->valuestring);
    
    json_item = cJSON_GetObjectItem(json_head,STR_CONTENT);
    json_item = cJSON_GetObjectItem(json_item,STR_FILE_NAME);
    cJSON_AddStringToObject(json_update,STR_FILE_NAME,           json_item->valuestring);

    cJSON_AddStringToObject(json_update,STR_FILE_TYPE,           (char *)upgrade_type);


    pack = cJSON_PrintUnformatted(json_update);
    printf("The JSON content:%s%s",pack,endchar);
    strcpy((char *)updatebuf, pack);
    if(pack){
        cJSON_free(pack);
    }
    cJSON_Delete(json_update);
    return 0;
}
/**
 * @brief       发送数据线程函数
 * @param       pvParameters : 传入struct link_socjet_info结构体
 * @description 创建UDP监听线程，将UDP接收到的消息处理好，返回应答Json数据帧
 * @retval      无
 */
#define	TCP_CLIENT_PORT			2688	//定义tcp client要连接的远端端口
#define	HTTP_CLIENT_PORT		9010	//定义tcp client要连接的远端端口
#define	SIZE_RECV_BUF			2000	//定义接收buf大小
__lwip_dev lwipdev;						//lwip控制结构体 
struct sockaddr_in atk_client_addr;
uint8_t g_file_receive_running=0;
uint8_t g_file_config_running=0;
/* 发送数据内容 */
uint8_t g_lwip_update_ack[3];


void t_upgrade(void *pvParameters)
{
	err_t err;
	//int g_lwip_connect_state = 0;
	uint16_t recv_data_len;
	uint16_t update_data_len = 0;
	uint8_t *lwip_update_recvbuf;
    uint32_t totalsize;
    uint32_t data_size;
    cJSON * json_head;
    cJSON * json_item;
    cJSON * json_value;
    uint8_t filepath[128];
    uint8_t filename[32];
    uint8_t filetype[] = "bin";
    uint8_t password[32];
    uint8_t MD5_recv_str[33];//本次升级传下来的MD5值
    int sock = -1;
    uint8_t software_version[10];
    LED_MODE(OTA_FILE_GETTING);
    uint16_t recvbuf_size = 1500;
	lwip_update_recvbuf = mem_malloc(recvbuf_size);

    json_head = cJSON_Parse((const char *)pvParameters);
    GWfree(pvParameters);

    if(json_head == NULL)
        goto err;

    json_item = cJSON_GetObjectItem(json_head,STR_CONTENT);
    json_value = cJSON_GetObjectItem(json_item,STR_FILE_PATH);
    //json_value = cJSON_GetArrayItem(json_value,0);
    strcpy((char *)filepath, json_value->valuestring);

    json_value = cJSON_GetObjectItem(json_item,STR_FILE_NAME);
    json_value = cJSON_GetArrayItem(json_value,0);
    strcpy((char *)filename, json_value->valuestring);

    json_value = cJSON_GetObjectItem(json_item,STR_MD5);
    json_value = cJSON_GetArrayItem(json_value,0);
    strcpy((char *)MD5_recv_str, json_value->valuestring);

    json_value = cJSON_GetObjectItem(json_item,STR_PASSWORD);
    json_value = cJSON_GetArrayItem(json_value,0);
    strcpy((char *)password, json_value->valuestring);


    //当前线程已被创建，无需再创建新线程  
    g_file_receive_running = 1;
	
	atk_client_addr.sin_family = AF_INET;                   /* 表示IPv4网络协议 */
	atk_client_addr.sin_port = htons(TCP_CLIENT_PORT);       /* 端口号 */
	atk_client_addr.sin_addr.s_addr = g_mqtt_ip_port.ip_addr.addr;   /* 远程IP地址 */
	sock = socket(AF_INET, SOCK_STREAM, 0);                 /* 可靠数据流交付服务既是TCP协议 */
	memset(&(atk_client_addr.sin_zero), 0, sizeof(atk_client_addr.sin_zero));
	
	/* 连接远程IP地址 */
	err = connect(sock, (struct sockaddr *)&atk_client_addr, sizeof(struct sockaddr));

	if (err == -1)
	{
        printf("TCP连接失败，退出升级流程\r\n");
        goto sock_err;
	}
    //分配接受缓冲区

    //循环发送升级请求
    do{
    /**
     * 发送一个升级请求，1S一次发送
    */
    vTaskDelay(1000);
    //_LwipFileAckPacket(err, lwip_update_recvbuf, "bin");
    _LwipFilePacket(0,lwip_update_recvbuf, STR_DEV_UPGRADE_REQ, filepath, filename, filetype, password);
    //json_head是MQTT传进来的升级请求
    //_LwipUpdatePacket(json_head, lwip_update_recvbuf, "bin");
    //写一个字节，读升级文件，附带目录和文件名
    printf("(uint8_t *)updatebuf is %s\r\n", (char *)lwip_update_recvbuf);
    err = write(sock, (char *)lwip_update_recvbuf, strlen((char *)lwip_update_recvbuf));


    /**
     * 等待接收到XTBOX下发应答
    */
    //等待XTBOX应答准备好升级文件
    memset(lwip_update_recvbuf, 0, recvbuf_size);
    recv_data_len = recv(sock,lwip_update_recvbuf,recvbuf_size,0);
    if (recv_data_len <= 0 )
    {
        printf("TCP连接失败，退出升级流程\r\n");
        goto sock_err;
    }
    printf("升级接收字符为：[0123] = \r\n*****\r\n%s\r\n****\r\n", lwip_update_recvbuf);
    /**
     * 处理XTBOX应答帧，
    */
    json_head = cJSON_Parse((const char *)lwip_update_recvbuf);
    json_item = cJSON_GetObjectItem(json_head,STR_RESPONSE);
    //XTBOX需要返回正确的JSON帧并且应答为OK
    }while( (json_head==NULL) || (json_item==NULL) || (strcmp(json_item->valuestring,"OK")) );
    
    json_item = cJSON_GetObjectItem(json_head,STR_FILE_SIZE);
    //获取总文件大小
    totalsize = json_item->valueint;
    json_item = cJSON_GetObjectItem(json_head,STR_DATA_SIZE);
    //获取总文件大小
    data_size = json_item->valueint;

    cJSON_Delete(json_head);
    printf("\r\ntotalsize = ****%d****\r\n", totalsize);
    printf("\r\ndata_size = ****%d****\r\n", data_size);

    //发送BUF动态申请，只有文件下发时需要用到该BUF，所以随用随申请
    while (1) 
	{
		vTaskDelay(100);
        LED0_TOGGLE();
        //接收1K升级文件，写入flash，recv_data_len为实际接收字节
        memset(lwip_update_recvbuf, 0, recvbuf_size);
        recv_data_len = recv(sock,lwip_update_recvbuf,recvbuf_size,0);
        if (recv_data_len <= 0 )
        {
            printf("TCP连接失败，退出升级流程\r\n");
			goto sock_err;
        }
        //如果时最后一包，然后又接收到了数据帧，则认为时结束帧，解析json包跳出循环，进行MD5校验
        if(update_data_len == (totalsize/data_size +1) )
        {
            json_head = cJSON_Parse((const char *)lwip_update_recvbuf);
            json_item = cJSON_GetObjectItem(json_head,STR_STATUS);
            if (!strcmp(json_item->valuestring,"done"))
            {
                //接收完成则跳出循环，返回响应帧
                _LwipFileAckPacket(err, lwip_update_recvbuf, "FILE_TRANSFER_DONE_REQ");
                err = write(sock, (char *)lwip_update_recvbuf, strlen((char *)lwip_update_recvbuf));
                printf("升级接收字符为：[0123] = \r\n****receive file over****\r\n");
                break;
            }
        }

        //当前接收的包数
        update_data_len = lwip_update_recvbuf[0]<<8 | lwip_update_recvbuf[1];
                    
        //flash大小为16M，0-0xFFFFFF范围，升级区域放在最后2M，1M备份，1M正常使用
        //recv_data_len正常为1024，最后一包时不足1024，也写入
        norflash_write(&lwip_update_recvbuf[2], 0xF00000+(update_data_len-1)*data_size, recv_data_len-2);
	
        //每次发送应答时，慢点发送，升级包慢慢传，给别的任务留时间
        vTaskDelay(100);
        g_lwip_update_ack[0] = lwip_update_recvbuf[0];
        g_lwip_update_ack[1] = lwip_update_recvbuf[1];
        //只能判断帧长度是否正确，recvbuf_size或者最后一包不足recvbuf_size，最后一包为文件大小对1024取余，加上最开始两个字节的包数
        g_lwip_update_ack[2] = 0;//0代表成功收到
        if( (recv_data_len != (data_size+2) ) && (recv_data_len != (totalsize%data_size + 2)) )
        {

            g_lwip_update_ack[2] = 1;//1代表有故障，接收到的帧的长度有问题，需要重发，重发后，直接写入flash 
        }
        
        //应答接收成功，或者需要重发
        err = write(sock, g_lwip_update_ack, sizeof(g_lwip_update_ack));
        
        
	    printf("升级接收字符为：[0123] = *****recv_data_len is 0x%x. 0x%x. 0x%x. 0x%x. 0x%x*****\r\n", recv_data_len, lwip_update_recvbuf[0],lwip_update_recvbuf[1], lwip_update_recvbuf[2], lwip_update_recvbuf[3]);
		
  }

    
    //接收完成后进行MD5校验，返回校验值，与MQTT下发的MD5值进行比较
    //接收到文件后，先将其存入flash中，然后统一进行校验
    //接收到字符串的MD5，需要跟算出来的MD5值进行比较
    uint8_t MD_BUF[16];

    //传入升级文件size即可计算MD5值，返回值放在MD_BUF中
    MD5(0xF00000,totalsize,MD_BUF);
    printf("升级接收字符为：[0123] = \r\n****totalsize is 0x%x****\r\n",totalsize);
    //写入flash最后16个字节
    //norflash_write(MD_BUF, 0xFFFFFF-16, sizeof(MD_BUF));
    uint8_t MD5_str[33];
    for(int i = 0; i < 17; i++)
    {
        uint8_t temp;
        temp = (MD_BUF[i]&0xF0)>>4;
        if(temp > 9)    MD5_str[0+2*i] = temp + 'a' - 10;
        else            MD5_str[0+2*i] = temp + '0';
        
        temp = MD_BUF[i]&0x0F;
        if(temp > 9)    MD5_str[1+2*i] = temp + 'a' - 10;
        else            MD5_str[1+2*i] = temp + '0';
    }
    //这里将MD5值转换为字符串，与传入的字符串进行比较，比较通过则升级
    MD5_str[32] = '\0';
    printf("MD5_calc_str  ***%s***\r\n", MD5_str);
    printf("MD5_recv_str  ***%s***\r\n", MD5_recv_str);
    
    if (!strcmp((char *)MD5_recv_str,(char *)MD5_str))
    {
        uint8_t temp = 0x5A;
        //比对正确则写升级标志，重启升级
        norflash_write((uint8_t *)&temp, 0x1000000-16-4-1, 1);
        norflash_write((uint8_t *)&totalsize, 0x1000000-16-4, 4);
        norflash_write((uint8_t *)&MD_BUF, 0x1000000-16, 16);
        printf("升级文件校验完成，准备重启升级\r\n");
        //系统软复位，执行bootloader
        sys_soft_reset();
    }
    printf("升级文件校验失败，退出升级流程\r\n");
sock_err:
    closesocket(sock);
    //升级完成，校验失败，则关闭所有连接，释放内存，等待关闭该任务
err:
    if (json_head)
    {
        cJSON_Delete(json_head);
    }
    mem_free(lwip_update_recvbuf);

    LED_MODE(LED_OFF);
    g_file_receive_running = 0;
    vTaskDelete(NULL);
    //升级文件第一帧会下发文件大小，保存起来
    //若升级MD5值比对不成功，则不进行升级
    //while(1)
    //{
    //    printf("MD5_str  ***%s***\r\n", MD5_str);
    //    vTaskDelay(1000);
    //}
}


void t_load_conf(void *pvParameters)
{
	err_t err;
	//int g_lwip_connect_state = 0;

	uint16_t recv_data_len;
	uint8_t *lwip_update_recvbuf;
    uint16_t recvbuf_size = 1500;
    cJSON * json_head;
    cJSON * json_item;
    cJSON * json_modbus_set;
    cJSON * json_register;
    cJSON * json_block;
    cJSON * json_value;
    int array_size;
    int i;
    uint8_t filepath[128] = {0};
    uint8_t filename[32] = {0};
    uint8_t filetype[] = "config";
    uint8_t password[32];
    uint8_t device_count=0,read_count=0,write_count=0;
    uint8_t config_version[10];
    LED_MODE(OTA_FILE_GETTING);


    int sock = -1;

    
	lwip_update_recvbuf = mem_malloc(SIZE_RECV_BUF);
    if(lwip_update_recvbuf == NULL)
    {
        printf("memory malloc failed!!!\r\n");
        goto err;
    }
    json_head = cJSON_Parse((const char *)pvParameters);
    GWfree(pvParameters);

    if(json_head == NULL)
    {
        printf("XTBOX格式错误，退出配置流程\r\n");
        goto err;
    }

    json_item = cJSON_GetObjectItem(json_head,STR_CONTENT);

    json_value = cJSON_GetObjectItem(json_item,STR_FILE_PATH);
    strcpy((char *)filepath, json_value->valuestring);
    json_value = cJSON_GetObjectItem(json_item,STR_FILE_NAME);
    json_value = cJSON_GetArrayItem(json_value, 0);
    strcpy((char *)filename, json_value->valuestring);
    strcpy((char *)filename, "modbus_conf.json");
    json_value = cJSON_GetObjectItem(json_item,STR_PASSWORD);
    json_value = cJSON_GetArrayItem(json_value,0);
    strcpy((char *)password, json_value->valuestring);
    cJSON_Delete(json_head);

    //当前线程已被创建，无需再创建新线程
    g_file_config_running = 1;
	
	atk_client_addr.sin_family = AF_INET;                   /* 表示IPv4网络协议 */
	atk_client_addr.sin_port = htons(TCP_CLIENT_PORT);       /* 端口号 */
	atk_client_addr.sin_addr.s_addr = g_mqtt_ip_port.ip_addr.addr;   /* 远程IP地址 */
	sock = socket(AF_INET, SOCK_STREAM, 0);                 /* 可靠数据流交付服务既是TCP协议 */
	memset(&(atk_client_addr.sin_zero), 0, sizeof(atk_client_addr.sin_zero));
	
	/* 连接远程IP地址 */
	err = connect(sock, (struct sockaddr *)&atk_client_addr, sizeof(struct sockaddr));

	if (err == -1)
	{
        printf("TCP连接失败，退出配置流程\r\n");
        goto sock_err;
	}
    //分配接受缓冲区

    printf("config area erasing start!!!\r\n");
    for (size_t i = PTR_MODBUS/4096; i <= END_MODBUS/4096; i++)
        norflash_erase_sector(i); 
    printf("config area erasing end!!!\r\n");
    //循环发送配置请求
    do{
    /**
     * 发送一个配置请求，1S一次发送
    */
    vTaskDelay(1000);
    //_LwipFileAckPacket(err, lwip_update_recvbuf, "bin");
    _LwipFilePacket(0,lwip_update_recvbuf, STR_DEV_UPGRADE_REQ, filepath, filename, filetype, password);

    //json_head是MQTT传进来的升级请求
    //_LwipUpdatePacket(json_head, lwip_update_recvbuf, "bin");
    //写一个字节，读升级文件，附带目录和文件名
    printf("(uint8_t *)lwip_update_recvbuf is %s\r\n", (char *)lwip_update_recvbuf);
    err = write(sock, (char *)lwip_update_recvbuf, strlen((char *)lwip_update_recvbuf));
    

    /**
     * 等待接收到XTBOX下发应答
    */
    //等待XTBOX应答准备好配置文件
    memset(lwip_update_recvbuf, 0, SIZE_RECV_BUF);
    recv_data_len = recv(sock,lwip_update_recvbuf,SIZE_RECV_BUF,0);
    if (recv_data_len <= 0 )
    {
        printf("TCP连接失败，退出配置流程\r\n");
        goto sock_err;
    }
    printf("配置文件字符为：[0123] = *****%s****\r\n", lwip_update_recvbuf);
    /**
     * 处理XTBOX应答帧，
    */
    json_head = cJSON_Parse((const char *)lwip_update_recvbuf);
    json_item = cJSON_GetObjectItem(json_head,STR_RESPONSE);
    //XTBOX需要返回正确的JSON帧并且应答为OK
    }while( (json_head==NULL) || (json_item==NULL) || (strcmp(json_item->valuestring,"OK")) );
    //XTBOX应答OK即可
    cJSON_Delete(json_head);

    //第一帧配置文件XTBOX应答之后，直接发送配置文件了
    /**
     * 开始接收终端设备数据
    */

    while (1) 
	{
        LED0_TOGGLE();
		vTaskDelay(10);
        //接收1K升级文件，写入flash，recv_data_len为实际接收字节
        //接收到type->config是终端modbus参数和地址等配置
        //接收到type->read/write是读写数据块
        memset(lwip_update_recvbuf, 0, SIZE_RECV_BUF);
        recv_data_len = recv(sock,lwip_update_recvbuf,SIZE_RECV_BUF,0);
        if (recv_data_len <= 0 )
        {
            printf("TCP连接失败，退出配置流程\r\n");
            goto sock_err;
        }
        printf("(uint8_t *)lwip_update_recvbuf is %s\r\n", (uint8_t *)lwip_update_recvbuf);

        //接收到后，判断是否是config还是read/write
        json_head = cJSON_Parse((const char *)lwip_update_recvbuf);
        //解析不正确，err返回1
        if(json_head==NULL) 
        {
            printf("XTBOX发送格式错误\r\n");
            err = 1;
        }
        
        json_item = cJSON_GetObjectItem(json_head,STR_TYPE);

        if(json_item == NULL)
        {
            json_item = cJSON_GetObjectItem(json_head,STR_DEVICE_COUNT);
            if(json_item)
            {
                device_count = json_item->valueint;
                //写入总终端个数
                norflash_write((uint8_t *)&device_count, ADDR_MODBUS_TABLE, 1);
                device_count=0;
                printf("(uint8_t *)device_count is write\r\n");
            }
            json_item = cJSON_GetObjectItem(json_head,STR_STATUS);
            if(json_item)
            {
                if (!strcmp(json_item->valuestring,"done"))
                {
                    _LwipFileAckPacket(0, lwip_update_recvbuf, NULL);
                    //返回ACK
                    printf("\r\n(uint8_t *)config ack is %s\r\n", (char *)lwip_update_recvbuf);
                    err = write(sock, (char *)lwip_update_recvbuf, strlen((char *)lwip_update_recvbuf));

                    //系统软复位，重新载入终端设备表
                    sys_soft_reset();
                }
                
            }
        }
        if (!strcmp(json_item->valuestring,STR_CONFIG))
        {
            STATUS_TABLE_HEAD p_config;
            //终端配置文件头，包含了modbus地址，参数等信息，解析后放入p头，填充后写入flash

            json_item = cJSON_GetObjectItem(json_head,STR_TERM_ID);
            strcpy((char *)p_config.term_id, json_item->valuestring);
            json_item = cJSON_GetObjectItem(json_head,STR_READ_COUNT);
            p_config.read_table_len = json_item->valueint;
            json_item = cJSON_GetObjectItem(json_head,STR_WRITE_COUNT);
            p_config.write_table_len = json_item->valueint;
            json_item = cJSON_GetObjectItem(json_head,STR_SLAVE_ID);
            p_config.slave_id = json_item->valueint;
            json_item = cJSON_GetObjectItem(json_head,STR_POLLING_TIME);
            p_config.polling_time = json_item->valueint;
            
            json_item = cJSON_GetObjectItem(json_head,STR_MODBUS_SET);
            json_modbus_set = cJSON_GetObjectItem(json_item,STR_BAUDRATE);
            p_config.baudrate = json_modbus_set->valueint;
            json_modbus_set = cJSON_GetObjectItem(json_item,STR_WORDLENGTH);
            p_config.wordlength = json_modbus_set->valueint;
            json_modbus_set = cJSON_GetObjectItem(json_item,STR_STOPBITS);
            p_config.stopbits = json_modbus_set->valueint;
            json_modbus_set = cJSON_GetObjectItem(json_item,STR_PARITY);
            p_config.parity = json_modbus_set->valueint;
            json_modbus_set = cJSON_GetObjectItem(json_item,STR_HWFLOWCTL);
            p_config.hwflowctl = json_modbus_set->valueint;

            p_config.read_reg_len_max = 0x1;//重新配置后，最大字节数需恢复默认，默认单寄存器单线圈读 
            //从第0个终端开始写
            norflash_write((uint8_t *)&p_config, ADDR_MODBUS_DEVICE_TABLE(device_count), sizeof(STATUS_TABLE_HEAD));
            device_count++;
            read_count = 0;
            write_count = 0;
            printf("(uint8_t *)p_config is write\r\n");
        }
        if (!strcmp(json_item->valuestring,STR_READ))
        {
            READ_TABLE p_read;
            
            //终端配置文件头，包含了modbus地址，参数等信息
            json_register = cJSON_GetObjectItem(json_head,STR_REGISTERS);
            array_size = read_count + cJSON_GetArraySize(json_register);
            for(i=0 ;read_count < array_size; read_count++ )
            {
                //终端配置文件头，包含了modbus地址，参数等信息
                json_block = cJSON_GetArrayItem(json_register, i++);
                
                json_value = cJSON_GetObjectItem(json_block,STR_STATUS);
                strcpy((char *)p_read.status, json_value->valuestring);
                json_value = cJSON_GetObjectItem(json_block,STR_FUNC_CODE);
                p_read.func_code = json_value->valueint;
                json_value = cJSON_GetObjectItem(json_block,STR_START_REG);
                p_read.start_reg = json_value->valueint;
                json_value = cJSON_GetObjectItem(json_block,STR_START_BIT);
                p_read.start_bit = json_value->valueint;
                json_value = cJSON_GetObjectItem(json_block,STR_BIT_LEN);
                p_read.bit_len = json_value->valueint;
                #if 0
                json_value = cJSON_GetObjectItem(json_block,STR_THRESHOLD_MIN);
                p_read.threshold_min = json_value->valueint;
                json_value = cJSON_GetObjectItem(json_block,STR_THRESHOLD_MAX);
                p_read.threshold_max = json_value->valueint;
                #endif
                json_value = cJSON_GetObjectItem(json_block,STR_THRESHOLD_CHANGE);
                p_read.threshold_change = json_value->valueint;

                norflash_write((uint8_t *)&p_read, ADDR_MODBUS_DEVICE_READ_TABLE(device_count-1, read_count, write_count), sizeof(p_read));
                //read_count++;
                printf("(uint8_t *)p_read is write\r\n");
            }
        }
        if (!strcmp(json_item->valuestring,STR_WRITE))
        {
            WRITE_TABLE p_write;

            //终端配置文件头，包含了modbus地址，参数等信息
            json_register = cJSON_GetObjectItem(json_head,STR_REGISTERS);
            array_size = write_count + cJSON_GetArraySize(json_register);

            for(i=0 ;write_count < array_size; write_count++ )
            {
                json_block = cJSON_GetArrayItem(json_register, i++);

                json_value = cJSON_GetObjectItem(json_block,STR_STATUS);
                strcpy((char *)p_write.status, json_value->valuestring);
                json_value = cJSON_GetObjectItem(json_block,STR_FUNC_CODE);
                p_write.func_code = json_value->valueint;
                json_value = cJSON_GetObjectItem(json_block,STR_START_REG);
                p_write.start_reg = json_value->valueint;
                json_value = cJSON_GetObjectItem(json_block,STR_START_BIT);
                p_write.start_bit = json_value->valueint;
                json_value = cJSON_GetObjectItem(json_block,STR_BIT_LEN);
                p_write.bit_len = json_value->valueint;

                norflash_write((uint8_t *)&p_write, ADDR_MODBUS_DEVICE_READ_TABLE(device_count-1, read_count, write_count), sizeof(p_write));
                //write_count++;
                printf("(uint8_t *)p_write is write\r\n");
            }

        }
        cJSON_Delete(json_head);
        _LwipFileAckPacket(err, lwip_update_recvbuf, NULL);
        //返回ACK
        printf("(uint8_t *)lwip_update_recvbuf is %s\r\n", (char *)lwip_update_recvbuf);
        err = write(sock, (char *)lwip_update_recvbuf, strlen((char *)lwip_update_recvbuf));
  }
sock_err:
    closesocket(sock);
err:
    if (json_head)
    {
        cJSON_Delete(json_head);
    }
    mem_free(lwip_update_recvbuf);
    LED_MODE(LED_OFF);
    g_file_config_running = 0;
    printf("\r\n ***t_load_conf is end!! ***\r\n");
    vTaskDelete(NULL);

    //升级文件第一帧会下发文件大小，保存起来
    //若升级MD5值比对不成功，则不进行升级
    //while(1)
    //{
    //    printf("\r\n ***t_load_conf is end!! ***\r\n");
    //    vTaskDelay(1000);
        
    //}

}


/**
 * @brief       mqtt进入数据回调函数
 * @param       arg：传入的参数
 * @param       data：数据
 * @param       len：数据大小
 * @param       flags：标志
 * @retval      无
 */
/* start: wangxiaoxu 20230607 lable_002 */
/* reason: add lwip Queue to transport data. */
xQueueHandle LwipQueue_to_Modbus;
xQueueHandle LwipQueue_to_Comm;
/* end: wangxiaoxu 20230607 lable_002 */

static void 
mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
    char * Lwip_send_to_485_buf;
    char * temp;
    LED_MODE(MQTT_DATA_TRANSLATE);
    LWIP_UNUSED_ARG(data);
    //printf("\r\nMQTT client \"%s\" data cb: len %d, flags %d\n", client_info->client_id,(int)len, (int)flags);
    /* start: wangxiaoxu 20230605 lable_001 */
    /* reason: send receive data to LwipQueue_to_Modbus */
    //printf("\r\n%s: \r\nMQTT client \"%s\" data: %s cb: len %d, flags %d\n", __FUNCTION__, client_info->client_id, data, (int)len, (int)flags);
    printf("\r\n%s: \r\nMQTT data: %s cb: len %d, flags %d\n", __FUNCTION__, data, (int)len, (int)flags);
    //当前flag为1，为MQTTx程序发布的消息
    if (flags & MQTT_DATA_FLAG_LAST == 0)
    {
        printf("当前消息不是MQTTx程序发送的消息\r\n");
        return;
    }
    /* start: wangxiaoxu 20230607 lable_002 */
    /* reason: for reasons. */
    if(LwipQueue_to_Modbus==NULL)
    {
        printf("等待LWIP队列初始化\r\n");
        return;
    }
    cJSON * json_head;
    cJSON * json_item;
    uint16_t array_size=0;
    
    //printf("Rec LORA data:%s\r\n",buff);
    //head = strstr((const char *)buff,str_head);
    //real = strstr((const char *)buff,str_real);
    json_head = cJSON_Parse((const char *)data);
    if(json_head==NULL){
        printf("发布消息格式错误，非JSON格式\r\n");
        return;
    }

    json_item = cJSON_GetObjectItem(json_head,STR_TIMESTAMP);
    if(json_item == NULL) printf("时间戳key值错误，本次不更新本机时间\r\n");
    else _UpdateTime(json_item->valueint);

    //不是本采集器ID的，只接收时间戳用于同步时间，其他丢弃
    json_item = cJSON_GetObjectItem(json_head,STR_DEV_ID);
    if( strcmp(json_item->valuestring,COLLECTORID)) 
    {
        printf("采集器ID错误\r\n");
        return;
    }
    
    json_item = cJSON_GetObjectItem(json_head,STR_CODE);
    if(json_item==NULL){
        printf("无code字段，请检查格式\r\n");
        return;
    }
    if (!strcmp(json_item->valuestring,STR_OTA_FILE_INFO))
    {
        json_item = cJSON_GetObjectItem(json_head,STR_CONTENT);
        if(json_item==NULL){
            printf("无content字段，请检查格式\r\n");
            return;
        }
        json_item = cJSON_GetObjectItem(json_item,STR_FILE_NAME);
        if(json_item==NULL){
            printf("无filename字段，请检查格式\r\n");
            return;
        }
        array_size = cJSON_GetArraySize(json_item);
        if(array_size==0){
            printf("filename内数组为空，本次不读取数据，请检查格式\r\n");
            return;
        }
        json_item = cJSON_GetArrayItem(json_item,0);
        //文件名第一个数据，判断是否存在conf后缀，有则返回指针，无则返回NULL
        if( strstr(json_item->valuestring, "conf"))
        {
            //创建新线程，配置文件下发，下发后直接写入flash
            if(!g_file_config_running)
            {
                printf("OTA配置下发任务创建\r\n");
                temp = GWMalloc(len+1);
                memcpy(temp, data, len);
                temp[len] = 0;
                t_load_conf_handler = sys_thread_new("t_load_conf", t_load_conf, (void *)temp, T_LOAD_CONF_STK_SIZE, T_LOAD_CONF_PRIO );
            }
            else{
                printf("OTA配置下发任务已经在运行中。。。\r\n");
            }
        }
        if( strstr(json_item->valuestring, "bin"))
        {
            //创建新线程，自动发现，XTBOX下发命令，设备回复
            //直接传入MQTT发送的升级帧，在线程里面处理
            //组帧后，线程直接发送即可，然后释放掉该json_update指针
            //如果线程未创建，则创建线程，重复收到，则不回应，或回复一个正在升级中
            if(!g_file_receive_running)
            {
                printf("OTA升级任务创建\r\n");
                temp = GWMalloc(len+1);
                memcpy(temp, data, len);
                temp[len] = 0;
                t_upgrade_handler = sys_thread_new("t_upgrade", t_upgrade, (void *)temp, T_UPGRADE_STK_SIZE, T_UPGRADE_PRIO );
            }
            else{
                printf("OTA升级任务已经在运行中。。。\r\n");
            }
        }
        //心跳中判断，如果升级任务完成或者配置表下发完成，则删除该tcp任务
        //vTaskDelete(t_publish_handler.thread_handle);
    }
    else if (!strcmp(json_item->valuestring,STR_QUERY_DEV_STATUS))
    {
        //增加宏定义MQTT_VAR_HEADER_BUFFER_LEN的值，可扩充接收缓冲区，需要malloc一段区域用于存储当前订阅消息
        //防止订阅消息过多，覆盖上次订阅的消息，处理完每一次的消息后，释放掉malloc的内存
        Lwip_send_to_485_buf = GWMalloc(len+1);
        memcpy(Lwip_send_to_485_buf, data, len);
        Lwip_send_to_485_buf[len] = 0;
        if(pdTRUE != xQueueSendToBack(LwipQueue_to_Modbus,(void *)&Lwip_send_to_485_buf,(TickType_t )10))
        {
            GWfree(Lwip_send_to_485_buf);
        }
    }
    else if (!strcmp(json_item->valuestring,STR_CONTROL_DEV_STATUS))
    {
        //控制命令需要放到队列最前面
        Lwip_send_to_485_buf = GWMalloc(len+1);
        memcpy(Lwip_send_to_485_buf, data, len);
        Lwip_send_to_485_buf[len] = 0;
        if(pdTRUE != xQueueSendToFront(LwipQueue_to_Modbus,(void *)&Lwip_send_to_485_buf,(TickType_t )10))
        {
            GWfree(Lwip_send_to_485_buf);
        }
    }
    else{
        printf("接收到不支持的code码\r\n");
    }

    if(json_head!=NULL){
        cJSON_Delete(json_head);
    }
        /* end: wangxiaoxu 20230607 lable_002 */
        /* end: wangxiaoxu 20230605 lable_001 */
        /* 从这里可以接收阿里云发布的数据 */ 
    
}

/**
 * @brief       mqtt进入发布回调函数
 * @param       arg：传入的参数
 * @param       topic：主题
 * @param       tot_len：主题大小
 * @retval      无
 */
static void
mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    //const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;

    //printf("\r\nMQTT client \"%s\" publish cb: topic %s, len %d\r\n", client_info->client_id,
    //      topic, (int)tot_len);
}

/**
 * @brief       mqtt发布回调函数
 * @param       arg：传入的参数
 * @param       err：错误值
 * @retval      无
 */
static void
mqtt_publish_request_cb(void *arg, err_t err)
{
    //printf("publish success\r\n");
}

/**
 * @brief       mqtt订阅响应回调函数
 * @param       arg：传入的参数
 * @param       err：错误值
 * @retval      无
 */
static void
mqtt_request_cb(void *arg, err_t err)
{
    //const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
    
    g_publish_flag = 1;
    //printf("\r\nMQTT client \"%s\" request cb: err %d\r\n", client_info->client_id, (int)err);
}

/**
 * @brief       mqtt连接回调函数
 * @param       client：客户端控制块
 * @param       arg：传入的参数
 * @param       status：连接状态
 * @retval      无
 */
static void
mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    err_t err;
    char subcribe_path[48];
    //const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
    
    LWIP_UNUSED_ARG(client);

    //printf("\r\nMQTT client \"%s\" connection cb: status %d\r\n", client_info->client_id, (int)status);

    /* 判断是否连接 */ 
    if (status == MQTT_CONNECT_ACCEPTED)
    {
        /* 判断是否连接 */ 
        if (mqtt_client_is_connected(client))
        {
            //成功建立连接，创建发布任务
            t_publish_handler = sys_thread_new("t_publish", t_publish, (void *)NULL, T_PUBLISH_STK_SIZE, T_PUBLISH_PRIO );

            g_err_flag = 0;//服务器连接成功清零故障标志

            //MQTT服务器建立连接，再创建向modbus发送消息的队列，队列由lwip控制，MQTT服务器断联后，不会向该队列继续发送消息了，所以可以不用管，因为只有incoming_data才会填充队列
            /* start: wangxiaoxu 20230605 lable_001 */
            /* reason: create 485 Queue send to Lwip */
            
            //vQueueDelete(LwipQueue_to_Modbus);mqtt服务器连接失败则删除队列，不进行接收消息，先禁用调度，将队列中指针都取出来释放掉，然后删除队列，再开启调度
            if(LwipQueue_to_Modbus == NULL){
                LwipQueue_to_Modbus = xQueueCreate(10,sizeof(char *));
                printf("Create LwipQueue_to_Modbus message queue!!!\n");
            }

            /* 设置传入发布请求的回调 */
            mqtt_set_inpub_callback(g_mqtt_client,
                                    mqtt_incoming_publish_cb,
                                    mqtt_incoming_data_cb,
                                    NULL);
            
            /* 订阅操作，并设置订阅响应会回调函数mqtt_sub_request_cb */ 
            sprintf(subcribe_path, DEVICE_SUBSCRIBE, COLLECTORID);
            err = mqtt_subscribe(client, subcribe_path, 1, mqtt_request_cb, arg);
            if(err != ERR_OK)
                printf("mqtt_subscribe error code: %d\n", err);
            else
                printf("mqtt_subscribe succeed!!\n");

        }
    }
    else/* 连接失败 */
    {
        printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);
    } 
}
/* start: wangxiaoxu 20230607 lable_002 */
/* reason: thread of publish 485 data. */
/**
 * @brief       发布消息publish线程函数
 * @param       pvParameters : 传入struct link_socjet_info结构体
 * @description 创建发布消息publish线程，用于接收485读上来的数据，发布到指令目录
 *              消息队列里面存放Json包的指针，数据结构由485构造，lwip模块只关心发送，传入该模块只有一个字符串的首地址
 * @retval      无
 */

//要把发心跳帧的LWIP模块休眠，发送线程若发送不出去，则存储在flash中，需要一直运行
extern TaskHandle_t T_Heart_Handler;             /* 任务句柄 */
void t_publish(void *pvParameters)
{
    int json_485_data_len = 0;
    uint8_t * json_485_data;
    cJSON * json_head = NULL;
    cJSON * json_item = NULL;
    
    uint8_t polling_flag = 0;
    uint8_t publish_err_flag = 0;
    
    uint8_t *filebuf = NULL;
    uint16_t filelen = 0;
    uint8_t *filename = NULL;
    xQueueHandle TempQueue;

    taskENTER_CRITICAL();           /* 进入临界区 */
	TempQueue = xQueueCreate(10,sizeof(char *));
    if(TempQueue == NULL){
        printf("Create LwipQueue_to_Comm message queue fail, cann't publish data!!!\n");
    }
    //创建任务，在连接回调里面，一定连接成功了，所以可以创建发布任务
    LwipQueue_to_Comm = TempQueue;
    taskEXIT_CRITICAL();            /* 退出临界区 */


    char publish_path[48];
    sprintf(publish_path, DEVICE_PUBLISH, COLLECTORID);

    while (1)
    {

        if(g_err_flag)
        {
            //如果心跳帧检测到故障，则停止接收
            LwipQueue_to_Comm = NULL;
            vTaskDelay(1000);
            continue;  
        }
        //如果没故障，并且队列都处理完成，则继续开始接收
        if( (g_err_flag == 0) && (uxQueueMessagesWaiting(TempQueue) == 0))
        {
            //故障恢复，则开始接收
            LwipQueue_to_Comm = TempQueue;
        }

        //1秒1次检测是否有数据发出，读取不移除数据，防止publish不成功
        if(xQueuePeek( TempQueue, &json_head, ( TickType_t ) 1000) == pdFALSE)
        {
            //上电后扫描flash，获取当前补报条数，当队列中没有向MQTT发送的数据时，发送补报数据
            //如果队列没接收到数据，并且此时没有补报数据需要发送，则提前返回
            //如果此时上报条数不等于0，则证明有数据需要上报
            vTaskDelay(10);
            continue;  
            if(get_polling_num("data") )
            {
                //读取最老的一条补报数据，返回剩余补报数据条数
                filename = "data";//先上报数据，后上报事件
                if(f_polling(&filebuf, &filelen, filename) == 0)
                {
                    //故障，清零总条数
                    reset_fs_ptr();
                    continue;
                }
            }
            else if(get_polling_num("event"))
            {
                filename = "event";
                if(f_polling(&filebuf, &filelen, filename) == 0)
                {
                    //故障，清零总条数
                    reset_fs_ptr();
                    continue;
                }
            }
            else{
                continue;
            }

            //判断读上来的文件，指针不为空，说明有数据读上来
            if(filebuf==NULL)
            {
                printf("读取历史记录失败\r\n");
                continue;
            }

            json_head = cJSON_Parse((char *)filebuf);
            //解析完立即释放文件buf
            cJSON_free(filebuf);
            filebuf = NULL;
            //如果读取上来，不是json字符串，则认为是无效数据
            if(json_head == NULL) 
            {
                printf("历史记录文件内容解析失败，丢弃该文件\r\n");
                f_polling_done(filename);
                //解析不成功，说明文件内容有误，可以在此删除掉错误文件，也可保留
                continue;
            }
            polling_flag = 1;
        }
        if (json_head == NULL)
        {
            printf("publish接收到错误字段\r\n");
            continue;
        }
        
        //先发送，发不出去再保存，判断是否有该字段，没有则需要新增
        json_item = cJSON_GetObjectItem(json_head, STR_DEV_ID);
        if(json_item == NULL)       cJSON_AddStringToObject(json_head,STR_DEV_ID,       COLLECTORID);
        
        json_item = cJSON_GetObjectItem(json_head, STR_SRC_TYPE);
        if(json_item == NULL)       cJSON_AddNumberToObject(json_head,STR_SRC_TYPE,     0);
        
        json_item = cJSON_GetObjectItem(json_head, STR_ENCRYPT);
        if(json_item == NULL)       cJSON_AddNumberToObject(json_head,STR_ENCRYPT,      0);

        json_item = cJSON_GetObjectItem(json_head, STR_TIMESTAMP);
        g_report_timestamp = GetTimeStamp();
        if(json_item == NULL)       cJSON_AddNumberToObject(json_head,STR_TIMESTAMP,     GetTimeStamp());
        else    cJSON_SetNumberValue(json_item, GetTimeStamp());

        json_485_data = (uint8_t *)cJSON_PrintUnformatted(json_head);
        //printf("the json_485_data is %s\n", json_485_data);
        json_485_data_len = strlen((char *)json_485_data);
        LED_MODE(MQTT_DATA_TRANSLATE);
        if(mqtt_publish(g_mqtt_client,publish_path,json_485_data,json_485_data_len,1,0,mqtt_publish_request_cb,NULL)!=ERR_OK){
            //发送不成功队列消息不移除
            printf("推送到MQTT服务器失败\r\n");
            publish_err_flag = 1;
            printf("the publish message is %s\n", json_485_data);
            cJSON_free(json_485_data);
            cJSON_Delete(json_head);
            filename = NULL;
            vTaskDelay(5000);
            continue;
        }
        else
        {    
            //补报数据，并且无故障，则认为上报成功
            if(polling_flag)
            {
                printf("补报数据成功，去掉补报标志位\r\n");
                f_polling_done(filename);
            }
            else{
                //发送成功后移除队列消息
                printf("modbus上报成功！！\r\n");
                xQueueReceive( TempQueue, &json_head, ( TickType_t ) 1);
            }
            printf("推送到MQTT服务器成功！\r\n");
            publish_err_flag = 0;
        }
        printf("the publish message is %s\n", json_485_data);
        cJSON_free(json_485_data);
        
        //modbus报过来的数据都认为是上报成功的数据，如果队列传递不过来，说明服务器出问题，队列暂停，在modbus模块保存历史数据未上报

        //清楚变量及缓冲区
        filename = NULL;
        cJSON_Delete(json_head);
        polling_flag = 0;
        vTaskDelay(10);
        //vTaskDelay(5000);
    }
}
/* end: wangxiaoxu 20230607 lable_002 */

/**
 * @brief       lwip_app进程
 * @param       无
 * @retval      无
 */

void lwip_app(void)
{
    err_t err;
    static struct mqtt_connect_client_info_t mqtt_client_info;
    /* start: wangxiaoxu 20230605 lable_001 */
    /* reason: dynamic modify ip address . */
    uint8_t *g_heart_beat;
    char publish_path[48];

    LED_MODE(MQTT_CONNECTED);
    /* end: wangxiaoxu 20230605 lable_001 */

    /* 设置一个空的客户端信息结构 */
    memset(&mqtt_client_info, 0, sizeof(mqtt_client_info));
    
    /* 设置客户端的信息量 */ 
    mqtt_client_info.client_id = (char *)CLIENT_ID;     /* 设备名称 */
    mqtt_client_info.client_user = NULL;
    mqtt_client_info.client_pass = NULL;
    mqtt_client_info.keep_alive = 100;                  /* 保活时间 */
    mqtt_client_info.will_msg = NULL;
    mqtt_client_info.will_qos = NULL;
    mqtt_client_info.will_retain = 0;
    mqtt_client_info.will_topic = 0;
    
    
    /* 创建MQTT客户端控制块 */
    g_mqtt_client = mqtt_client_new();
    
    /* start: wangxiaoxu 20230607 lable_002 */
    /* reason: after MQTT connection, create a thread for publishing Json packge of 485 data. */
    err = 1;
    //等待连接完成，每5s检测一次
    printf("开始连接MQTT服务器\r\n");

    norflash_read((char *)&(g_mqtt_ip_port), ADDR_MQTT_IP, sizeof(g_mqtt_ip_port));
    //norflash_read((char *)&(g_mqtt_ip_port.port), ADDR_MQTT_PORT, SIZE_MQTT_PORT);
    printf("MQTT address is 0x%x\r\n", g_mqtt_ip_port.ip_addr.addr);
    printf("MQTT port is %d\r\n", g_mqtt_ip_port.port);

    for (size_t i = 0; i < 3; i++)
    {
        err = mqtt_client_connect(g_mqtt_client,        /* 服务器控制块 */
                            &g_mqtt_ip_port.ip_addr, g_mqtt_ip_port.port,/* 服务器IP与端口号 */
                            mqtt_connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),/* 设置服务器连接回调函数 */
                            &mqtt_client_info); /* MQTT连接信息 */
        vTaskDelay(50);
        if(err == 0) break;//3次内连接成功，则不进行设备发现，不启动UDP和TCP任务
    }
    lwip_device_receive_thread();//创建UDP和TCP接收任务，用于随时更新MQTT的IP和PORT

    if(err || (g_mqtt_ip_port.port == 0xFFFF))
    {
        printf("设备自动发现流程开始\r\n");
        //如果3次都连接失败，则执行设备自动发现流程
        lwip_device_online_declare();
        /* end: wangxiaoxu 20230605 lable_001 */

        //等待网络配置成功，再进行之后的发布订阅过程。
        printf("等待设备自动发现成功配置\r\n");
        while(g_MQTT_netconfig_complete_flag != NET_CONFIG_OK)
        {
            vTaskDelay(50);
        }
        printf("XTBOX下发网络配置成功！\r\n");

        while(err)
        {
            err = mqtt_client_connect(g_mqtt_client,        /* 服务器控制块 */
                                &g_mqtt_ip_port.ip_addr, g_mqtt_ip_port.port,/* 服务器IP与端口号 */
                                mqtt_connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),/* 设置服务器连接回调函数 */
                                &mqtt_client_info); /* MQTT连接信息 */
            vTaskDelay(5000);
        }
    }
    printf("连接MQTT服务器成功\r\n");
    //第一次上电的心跳帧，需要附带版本号，后续发布版本需要增加上
    //g_version_flag = 0xFFFF;
    //连接MQTT服务器之后再创建发布任务，上电时的数据保存起来作为未上报数据
    /* end: wangxiaoxu 20230607 lable_002 */


    sprintf(publish_path, DEVICE_PUBLISH, COLLECTORID);
    while(1)
    {

        //判断是否为硬件连接失效，还是MQTT服务器失效
        if(err != 0)
        {
            //vTaskSuspend(t_publish_handler.thread_handle);
            if(g_lwipdev.link_status == LWIP_LINK_OFF)
            {
                //网线拔掉，记录日志
                printf("\r\nthe net cable is link down!!!\r\n");
                g_err_flag |= 0x01;
            }
            else{
                //MQTT服务器失效，记录日志
                printf("\r\nthe MQTT server is disconnect!!!\r\n");
                g_err_flag |= 0x02;
            }
            printf("MQTT服务器连接失败\r\n");
            //计算断线时长
            g_version_flag = 0;
            while(err != 0)
            {
                err = mqtt_client_connect(g_mqtt_client,        /* 服务器控制块 */
                                &g_mqtt_ip_port.ip_addr, g_mqtt_ip_port.port,/* 服务器IP与端口号 */
                                mqtt_connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),/* 设置服务器连接回调函数 */
                                &mqtt_client_info); /* MQTT连接信息 */
                vTaskDelay(5000);
                g_version_flag ++;
            }
            printf("MQTT服务器恢复\r\n");

            if(g_lwipdev.link_status == LWIP_LINK_ON)
            {
                //记录网线插入日志
            }
            else{
                //记录MQTT服务器恢复日志
            }
            //vTaskResume(t_publish_handler.thread_handle);
        }
        vTaskDelay(g_heart_beat_time * 1000);
        /* end: wangxiaoxu 20230607 lable_002 */

        if(GetTimeStamp() < g_report_timestamp + g_heart_beat_time)
            continue;
        /* start: wangxiaoxu 20230607 lable_002 */
        /* reason: modify lwip_demo host thread to send heart beat per 3 second. */
        //主流程用作心跳帧，3S一次，可配置    
        //sprintf((char *)g_heart_beat, "{\"the sdc is online, connneted to XTBOX! please check out device status OK!!\":\"}");
        g_heart_beat = _LwipHeartBeatPacket();
        g_heart_beat_len = strlen((char *)g_heart_beat);
        LED_MODE(MQTT_DATA_TRANSLATE);
        if(mqtt_publish(g_mqtt_client,publish_path,g_heart_beat,g_heart_beat_len,1,0,mqtt_publish_request_cb,NULL)!=ERR_OK)
        {
            if(mqtt_client_is_connected(g_mqtt_client)==0) 
            {
                err = -1;
                printf("心跳帧发送失败\r\n");
            }
        }
        else
        {
            g_err_flag = 0;//每次心跳帧发送成功则清零
        }

        if(g_heart_beat) cJSON_free(g_heart_beat);


    }
}
/* end: wangxiaoxu 20230605 lable_001 */




//组帧设备上线的Json包
uint8_t _LwipDeviceOnlinePacket(uint8_t *send_buf)
{
    cJSON * json_head;
    char *pack;
    char *endchar = "\r\n";
    json_head = cJSON_CreateObject();
    cJSON_AddStringToObject(json_head,STR_CMD,           STR_DEV_ONLINE);
    cJSON_AddNumberToObject(json_head,STR_TIMESTAMP,     GetTimeStamp());
    cJSON_AddStringToObject(json_head,STR_SRC_ID,        COLLECTORID);
    cJSON_AddNumberToObject(json_head,STR_ENCRYPT,       0);
    cJSON_AddStringToObject(json_head,STR_IP,            ip4addr_ntoa(&g_lwip_netif.ip_addr));
    cJSON_AddNumberToObject(json_head,STR_PORT,          2020);
    pack = cJSON_PrintUnformatted(json_head);
    printf("The JSON content:%s%s",pack,endchar);
    strcpy((char *)send_buf, pack);
    if(pack){
        cJSON_free(pack);
    }
    cJSON_Delete(json_head);
    return 0;
}

//组帧XTBOX上线应答Json包
uint8_t _LwipOnlineAckPacket(char *send_buf)
{
    cJSON * json_head;
    char *pack;
    char *endchar = "\r\n";
    json_head = cJSON_CreateObject();
    cJSON_AddStringToObject(json_head,STR_CMD,           STR_ONLINE_ACK);
    cJSON_AddNumberToObject(json_head,STR_TIMESTAMP,     GetTimeStamp());
    cJSON_AddStringToObject(json_head,STR_SRC_ID,        COLLECTORID);
    cJSON_AddNumberToObject(json_head,STR_ENCRYPT,       0);
    cJSON_AddStringToObject(json_head,STR_IP,            ip4addr_ntoa(&g_lwip_netif.ip_addr));
    pack = cJSON_PrintUnformatted(json_head);
    printf("The JSON content:%s%s",pack,endchar);
    strcpy(send_buf, pack);
    if(pack){
        cJSON_free(pack);
    }
    cJSON_Delete(json_head);
    return 0;
}

//组帧设备网络配置应答Json包
uint8_t _LwipNetAckPacket(char *send_buf)
{
    cJSON * json_head;
    char *pack;
    char *endchar = "\r\n";
    json_head = cJSON_CreateObject();
    cJSON_AddStringToObject(json_head,STR_CMD,           STR_NET_CONF_ACK);
    cJSON_AddNumberToObject(json_head,STR_TIMESTAMP,     GetTimeStamp());
    cJSON_AddStringToObject(json_head,STR_SRC_ID,        COLLECTORID);
    cJSON_AddNumberToObject(json_head,STR_RESULT,        0);
    pack = cJSON_PrintUnformatted(json_head);
    printf("The JSON content:%s%s",pack,endchar);
    strcpy(send_buf, pack);
    if(pack){
        cJSON_free(pack);
    }
    cJSON_Delete(json_head);
    return 0;
}

//处理传入Lwip模块的数据，根据cmd解析Json包
uint8_t LwipDataProcess(char * recv_buf, char * send_buf)
{
	cJSON * json_head;
	cJSON * json_item;
    static uint64_t online_time_stamp = 0;
    static uint64_t netconf_time_stamp = 0;
	
	json_head = cJSON_Parse(recv_buf);
    if(json_head==NULL) return 0;
    //每次接收到Json包，先解析时间戳，更新时间，然后处理应答，如果是网络配置，则更新MQTT网络配置，置网络配置完成标志位
    json_item = cJSON_GetObjectItem(json_head,STR_TIMESTAMP);
    _UpdateTime(json_item->valueint);

    json_item = cJSON_GetObjectItem(json_head,STR_CMD);

    //临时测试使用
    if (!strcmp(json_item->valuestring,STR_CONTROL_DEV_STATUS))
    {
        //创建新线程，自动发现，XTBOX下发命令，设备回复
        /*cJSON *json_update;
        //uint8_t *updatebuf;
        //updatebuf = mem_malloc(LWIP_SEND_BUF_SIZE);
        //_LwipUpdatePacket(json_head, updatebuf);*/
        //组帧后，线程直接发送即可，然后释放掉该json_update指针
        //t_upgrade_handler = sys_thread_new("t_upgrade", t_upgrade, (void *)NULL, T_UPGRADE_STK_SIZE, T_UPGRADE_PRIO );
        //t_load_conf_handler = sys_thread_new("t_load_conf", t_load_conf, (void *)NULL, T_LOAD_CONF_STK_SIZE, T_LOAD_CONF_PRIO );
        
        //心跳中判断，如果升级任务完成或者配置表下发完成，则删除该tcp任务
        //vTaskDelete(t_publish_handler.thread_handle);
    }
    //临时测试使用


    if (strcmp(json_item->valuestring, STR_ONLINE)==0)
    {
        if(GetTimeStamp() - online_time_stamp >= 15)
        {
            online_time_stamp = GetTimeStamp();
            printf("Receive XTBOX online message!!!\r\n");

            json_item = cJSON_GetObjectItem(json_head,STR_IP);
            //g_mqtt_ip_port.ip_addr.addr = inet_addr(json_item->valuestring);
            g_xtbox_ip_port.ip_addr.addr = inet_addr(json_item->valuestring);
            printf("net config addr is %s\r\n", json_item->valuestring);

            json_item = cJSON_GetObjectItem(json_head,STR_PORT);
            //g_mqtt_ip_port.port = json_item->valueint;
            //g_mqtt_ip_port.port = 1883;
            g_xtbox_ip_port.port = json_item->valueint;
            printf("net config port is 0x%x\r\n", json_item->valueint);
            g_MQTT_netconfig_complete_flag = NET_CONFIG_START;
            _LwipOnlineAckPacket(send_buf);
            //return 1;
        }
    }
    else if (strcmp(json_item->valuestring, STR_NET_CONF)==0)
    {
        if(GetTimeStamp() - netconf_time_stamp >= 15)
        {
            netconf_time_stamp = GetTimeStamp();
            printf("Receive net config message!!!\r\n");
            
            #if 0
            //以下是设置本地设备的网卡参数，暂时用不到，设置的函数先放在这里，以后可能会用到
            netif_set_down(&g_lwip_netif); //先禁用网卡
            //netif_set_gw(&g_lwip_netif, &GW_updata);        //重新设置网关地址
            //netif_set_netmask(&g_lwip_netif, &Mask_update); //重新设置子网掩码
            netif_set_ipaddr(&g_lwip_netif, &inet_addr(json_item->valuestring));    //重新设置IP地址
            //netif_set_addr(&g_lwip_netif, &ip_update, &Mask_update, &GW_updata);  //同时设置IP地址，掩码，网关
            netif_set_up(&g_lwip_netif);  //启用网卡
            #endif
            json_item = cJSON_GetObjectItem(json_head,STR_BROKER);
            printf("the 'BROKER' item is %s\r\n", json_item->valuestring);
            char buf[15] = {0};
            uint8_t i = 0;
            char index = 0;
            //去掉IP地址之前的字符，直到遇到数字，认为是IP地址
            while( (json_item->valuestring[index] <= '0') || (json_item->valuestring[index] >= '9') )
                index++;

            //截取BROKER中的IP地址
            while( json_item->valuestring[index] != ':' )
            {
                buf[i] = json_item->valuestring[index];
                i++;
                index++;
            }
            //赋值到另一个buf中，转换地址给采集器
            g_mqtt_ip_port.ip_addr.addr = inet_addr(buf);
            //跳过冒号
            index++;
            i = 0;
            //最后面几个为port端口，也赋值到buf中进行转换
            while( json_item->valuestring[index] != '\0' )
            {
                buf[i] = json_item->valuestring[index];
                index++;
                i++;
            }
            buf[i]=0;
            //此时buf中已有port端口,1883->i=4
            g_mqtt_ip_port.port = atoi(buf);
            //for( index = 0; index < i; index++ )
            //{
                //递归将1883端口字符串，转换为数字1883
            //    g_mqtt_ip_port.port += ( (buf[index] - '0') + (g_mqtt_ip_port.port*10) );
            //}
            
            printf("MQTT address is 0x%x\r\n", g_mqtt_ip_port.ip_addr.addr);
            printf("MQTT port is %d\r\n", g_mqtt_ip_port.port);
            norflash_write((char *)&(g_mqtt_ip_port), ADDR_MQTT_IP, sizeof(g_mqtt_ip_port));
            //norflash_write((char *)&(g_mqtt_ip_port.port), ADDR_MQTT_PORT, SIZE_MQTT_PORT);
            g_MQTT_netconfig_complete_flag = NET_CONFIG_OK;
            _LwipNetAckPacket(send_buf);
            //return 1;
        }
    }
		else{
			return 0;
		}
	

	
	if(json_head!=NULL){
		cJSON_Delete(json_head);
	}

	return 1;
}


//传入socket绑定到传入的组播IP，返回socket描述符
void bind_local_socket_to_ip(struct link_socjet_info *socket_info, struct ip_mreq_t *mreq_info, const char *udp_ip, uint16_t udp_port)
{
    socket_info->client_addr.sin_family = AF_INET;
    socket_info->client_addr.sin_addr.s_addr = inet_addr(udp_ip);   /* 待与 socket 绑定的广播网络接口 IP */   
    socket_info->client_addr.sin_port = htons(udp_port);                /* 待与 socket 绑定的广播端口号 */
    socket_info->client_addr_len = sizeof(socket_info->client_addr);
    
    /* 设置接收和发送缓冲区 */
    socket_info->recv.buf = g_lwip_demo_recvbuf;
    socket_info->recv.size = sizeof(g_lwip_demo_recvbuf);
    
    /* 将 Socket 与本地某网络接口绑定 */
    int ret = bind(socket_info->sfd, (struct sockaddr*)&socket_info->client_addr, socket_info->client_addr_len);
    
    if (ret < 0)
    {
        printf(" bind error!\n ");
    }
    
    mreq_info->mreq.imr_multiaddr.s_addr = inet_addr(udp_ip);     /* 多播组 IP 地址设置 */
    mreq_info->mreq.imr_interface.s_addr = htonl(INADDR_ANY);       /* 待加入多播组的 IP 地址 */
    mreq_info->mreq_len = sizeof(struct ip_mreq);

    /* 添加多播组成员（该语句之前，socket 只与 某单播IP地址相关联 执行该语句后 将与多播地址相关联） */
    ret = setsockopt(socket_info->sfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq_info->mreq,mreq_info->mreq_len);
    
    if (ret < 0)
    {
        printf("setsockopt failed !");
    }
    else
    {
        printf("setsockopt success\n");
    }

}

/**
 * @brief       发送数据线程函数
 * @param       pvParameters : 传入struct link_socjet_info结构体
 * @description 创建UDP监听线程，将UDP接收到的消息处理好，返回应答Json数据帧
 * @retval      无
 */

void t_udp_thread(void *pvParameters)
{
    struct link_socjet_info *socket_info = pvParameters;
    
    int length = 0;
    struct sockaddr_in sender;
    int sender_len = sizeof(sender);
    
    while (1)
    {
        socket_info->client_addr.sin_addr.s_addr = inet_addr(GROUP_IP);   /* 待与 socket 绑定的广播网络接口 IP */   
        socket_info->client_addr.sin_port = htons(GROUP_PORT);                /* 待与 socket 绑定的广播端口号 */

        //接收数据阻塞，需要接收BUF存放数据，暂时设置1024字节缓存，接收到XTBOX发送的上线消息后，发送设备的ACK
        length = recvfrom(socket_info->sfd,socket_info->recv.buf,socket_info->recv.size,0,(struct sockaddr*)&sender,(socklen_t *)&sender_len);
        if (length < 0)
            continue;
        
        socket_info->recv.buf[length]='\0';
        printf("%s %d : %s\n", inet_ntoa(sender.sin_addr), ntohs(sender.sin_port), socket_info->recv.buf);
        

        socket_info->client_addr.sin_addr.s_addr = inet_addr(DECLARE_IP);   /* 待与 socket 绑定的广播网络接口 IP */   
        socket_info->client_addr.sin_port = htons(DECLARE_PORT);                /* 待与 socket 绑定的广播端口号 */

        //发送BUF动态申请，因为只存在于上线，网络配置时需要发送数据，大部分时间都处于阻塞状态，不需要占用内存
        socket_info->send.buf = (char *)mem_malloc(LWIP_SEND_BUF_SIZE);
        if(LwipDataProcess(socket_info->recv.buf, socket_info->send.buf))
        {
            /* 数据广播，本次有数据发送，则发送应答帧 */
            sendto(socket_info->sfd, socket_info->send.buf, strlen(socket_info->send.buf), 0, (struct sockaddr*)&socket_info->client_addr,socket_info->client_addr_len);
        }
        mem_free(socket_info->send.buf);
        //printf("Lwip_ack is sending!!!! \r\n");
    }
}



/**
 * @brief       本地IP监听线程函数
 * @param       pvParameters : 传入struct link_socjet_info结构体
 * @description 创建UDP监听线程，将UDP接收到的消息处理好，返回应答Json数据帧
 * @retval      无
 */
void t_tcp_thread(void *pvParameters)
{
    pvParameters = pvParameters;
    struct link_socjet_info *socket_info;
    int length = 0;
    struct sockaddr_in sender;
    int sender_len = sizeof(sender);
    socket_info = mem_malloc(sizeof(struct link_socjet_info));
    socket_info->sfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_info->sfd < 0)
    {
        printf("socket failed!\n");
    }
    //进程运行太快了，到这里需要等到online结束
    socket_info->client_addr.sin_family = AF_INET;
    socket_info->client_addr.sin_addr.s_addr = g_lwipdev.ip[0] | g_lwipdev.ip[1] << 8 | g_lwipdev.ip[2] << 16 | g_lwipdev.ip[3] << 24;   /* 监听本地IP */   
    socket_info->client_addr.sin_port = htons(2020);                /* 本地监听端口号为2020 */
    socket_info->client_addr_len = sizeof(socket_info->client_addr);
    
    /* 设置接收和发送缓冲区 */
    socket_info->recv.buf = g_lwip_demo_recvbuf;
    socket_info->recv.size = sizeof(g_lwip_demo_recvbuf);
    
    /* 将 Socket 与本地某网络接口绑定 */
    int ret = bind(socket_info->sfd, (struct sockaddr*)&socket_info->client_addr, socket_info->client_addr_len);
    
    if (ret < 0)
    {
        printf(" bind error!\n ");
    }

    while (1)
    {
        //接收数据阻塞，需要接收BUF存放数据，暂时设置1024字节缓存，接收到XTBOX发送的上线消息后，发送设备的ACK
        length = recvfrom(socket_info->sfd,socket_info->recv.buf,socket_info->recv.size,0,(struct sockaddr*)&sender,(socklen_t *)&sender_len);
        if (length < 0)
            continue;
        
        socket_info->recv.buf[length]='\0';
        printf("%s %d : %s\n", inet_ntoa(sender.sin_addr), ntohs(sender.sin_port), socket_info->recv.buf);

        socket_info->client_addr.sin_addr.s_addr = g_xtbox_ip_port.ip_addr.addr;   /* 向online回复的XTBOX IP和端口应答netconf */   
        socket_info->client_addr.sin_port = htons(g_xtbox_ip_port.port);                /* 端口号 */

        //发送BUF动态申请，因为只存在于上线，网络配置时需要发送数据，大部分时间都处于阻塞状态，不需要占用内存
        socket_info->send.buf = (char *)mem_malloc(LWIP_SEND_BUF_SIZE);
        LwipDataProcess(socket_info->recv.buf, socket_info->send.buf);
        /* 数据广播 */
        sendto(socket_info->sfd, socket_info->send.buf, strlen(socket_info->send.buf), 0, (struct sockaddr*)&socket_info->client_addr,socket_info->client_addr_len);
        mem_free(socket_info->send.buf);
        //printf("Lwip_ack is sending!!!! \r\n");
    }
}



//设备上线声明，初始化上电后和网络连接断掉重连后，执行此函数，用于声明设备上线
void lwip_device_receive_thread(void)
{
    int ret;
    struct link_socjet_info *socket_info_read;
    struct ip_mreq_t *mreq_info_read;

    socket_info_read = mem_malloc(sizeof(struct link_socjet_info));
    mreq_info_read = mem_malloc(sizeof(struct ip_mreq_t));
    socket_info_read->sfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_info_read->sfd < 0)
    {
        printf("socket failed!\n");
    }

    //绑定接收端口
    bind_local_socket_to_ip(socket_info_read, mreq_info_read, GROUP_IP, GROUP_PORT);

    //创建新线程，自动发现，XTBOX下发命令，设备回复
    t_udp_handler = sys_thread_new("t_udp_thread", t_udp_thread, (void *)socket_info_read, T_UDP_STK_SIZE, T_UDP_PRIO );
    printf("UDP组播任务创建\r\n");
    t_tcp_handler = sys_thread_new("t_tcp_thread", t_tcp_thread, NULL, T_TCP_STK_SIZE, T_TCP_PRIO );
    printf("TCP组播任务创建\r\n");
}

//设备上线声明，初始化上电后和网络连接断掉重连后，执行此函数，用于声明设备上线
void lwip_device_online_declare(void)
{
    int ret;
    struct link_socjet_info *socket_info_write;
    struct ip_mreq_t *mreq_info_write;

    socket_info_write = mem_malloc(sizeof(struct link_socjet_info));
    mreq_info_write = mem_malloc(sizeof(struct ip_mreq_t));
    socket_info_write->sfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (socket_info_write->sfd < 0)
    {
        printf("socket failed!\n");
    }
        //绑定接收端口
    bind_local_socket_to_ip(socket_info_write, mreq_info_write, DECLARE_IP, DECLARE_PORT);

    //如果30s没收到XTBOX发布的消息，则退出，如果收到XTBOX的online消息，则是开始netconfig状态，不发送dev_online但等待netconfig
    while(g_MQTT_netconfig_complete_flag == NET_CONFIG_NONE)
    {
        printf("未收到XTBOX在线声明\r\n");
        socket_info_write->send.buf = (char *)mem_malloc(LWIP_SEND_BUF_SIZE);
        _LwipDeviceOnlinePacket((uint8_t *)socket_info_write->send.buf);
        sendto(socket_info_write->sfd, socket_info_write->send.buf, strlen(socket_info_write->send.buf), 0, (struct sockaddr*)&socket_info_write->client_addr,socket_info_write->client_addr_len);
        mem_free(socket_info_write->send.buf);
        //printf("Device send online message!!!");
        vTaskDelay(30000);
    }
}
/* end: wangxiaoxu 20230605 lable_001 */

/**
 * @brief      将16进制数转化为字符串
 * @param      pbSrc - 输入16进制数的起始地址
 * @param      nLen - 16进制数的字节数
 * @param       pbDest - 存放目标字符串
 * @retval     无
 */
void lwip_ali_hextostr(uint8_t *pbDest, uint8_t *pbSrc, int nLen)
{
    char ddl, ddh;
    int i;

    for (i = 0; i < nLen; i++)
    {
        ddh = 48 + pbSrc[i] / 16;
        ddl = 48 + pbSrc[i] % 16;

        if (ddh > 57) ddh = ddh + 7;

        if (ddl > 57) ddl = ddl + 7;

        pbDest[i * 2] = ddh;
        pbDest[i * 2 + 1] = ddl;
    }

    pbDest[nLen * 2] = '\0';
}

/**
 * @brief      通过hmac_sha1算法获取password
 * @param      device_secret---设备的密钥
 * @param      content --- 登录密码
 * @param      password---返回的密码值
 * @retval     无
 */
void lwip_ali_get_password(const char *device_secret, const char *content, char *password)
{
    char buf[256] = {0};
    int len = sizeof(buf);
    hmac_sha1((uint8_t *)device_secret, strlen(device_secret), (uint8_t *)content, strlen(content), (uint8_t *)buf, (unsigned int *)&len);
    lwip_ali_hextostr((uint8_t *)password, (uint8_t *)buf, len);
}

