

#ifndef _SYS_H
#define _SYS_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"


/**
 * SYS_SUPPORT_OS用于定义系统文件夹是否支持OS
 * 0,不支持OS
 * 1,支持OS
 */
#define SYS_SUPPORT_OS         1

//#define COLLECTORID ((char *)0x0807FD00)
extern uint8_t COLLECTORID[];
#define MAC_ADDRESS(x) (*((volatile uint8_t *)0x0807FD20+(x)))
#define MAC_ADDR (*((volatile uint32_t *)0x0807FD20))


#define VERSION_SOFTWARE "V3.0.0"
#define VERSION_HARDWARE "V3.0.0"


//枚举UDP组播消息的索引，用于Json格式的解析
#define    STR_DEV_INFO             "dev_info"      //设备信息
#define    STR_HARDWARE_VERSION     "hw_ver"  		//硬件版本
#define    STR_SOFTWARE_VERSION     "sf_ver"  		//软件版本
#define    STR_DATA_VERSION         "cf_ver"      	//配置文件版本号

#define    STR_REPORT_LINK_STATE     "REPORT_LINK_STATE"    //发布设备在线状态           
#define    STR_QUERY_LINK_STATE      "QUERY_LINK_STATE"     //查询设备在线状态        
#define    STR_OTA_FILE_INFO         "OTA_FILE_INFO"        //发布配置文件更新        
#define    STR_OTA_FILE_FINISH       "OTA_FILE_FINISH"      //配置文件下载完成通知        
#define    STR_REPORT_DEV_STATUS     "REPORT_DEV_STATUS"    //发布设备工作状态            
#define    STR_QUERY_DEV_STATUS      "QUERY_DEV_STATUS"     //查询设备工作状态            
#define    STR_CONTROL_DEV_STATUS    "CONTROL_DEV_STATUS"   //控制设备工作状态            
#define    STR_QUERY_ONLINE_DEV      "QUERY_ONLINE_DEV"     //查询在线设备        
#define    STR_REPORT_ONLINE_DEV     "REPORT_ONLINE_DEV"    //发布在线设备列表            
#define    STR_QUERY_DEV_INFO        "QUERY_DEV_INFO"       //查询设备信息        
#define    STR_REPORT_DEV_INFO       "REPORT_DEV_INFO"      //发布设备信息         
#define    STR_QUERY_ALL_MSG         "QUERY_ALL_MSG"        //查询所有设备信息     
#define    STR_CLEAR_ALL_DATA        "CLEAR_ALL_DATA"       //恢复出场设置     
#define    STR_REPORT_DEV_WARNING    "REPORT_DEV_WARNING"   //上报当前设备告警
#define    STR_REPORT_HISTORY_DATA   "REPORT_HISTORY_DATA"  //上报历史数据     
#define    STR_REPORT_HISTORY_EVENT  "REPORT_HISTORY_EVENT" //上报历史事件     

#define    STR_DEV_UPGRADE_REQ       "DEV_UPGRADE_REQ"      //给XTBOX发送升级请求     
#define    STR_DEV_CONFIG_REQ        "DEV_CONFIG_REQ"       //给XTBOX发送配置文件请求    



#define    NUM_NO_ENCRYPT          0    //不加密
#define    NUM_DES_ENCRYPT         1    //DES加密
#define    NUM_RSA_ENCRYPT         2    //RSA加密

#define    ERR_NO_ERR                   0   //设备无故障
#define    ERR_OFF_LINE	                1   //网络掉线
#define    ERR_LOW_BATTERY	            2   //电量低
#define    ERR_NO_FILES	                3   //文件不存在
#define    ERR_LINK_TIMEOUT	            4   //连接超时
#define    ERR_FTP_TIMEOUT	            5   //FTP下载文件超时
#define    ERR_FILE_CHECK_ERR	        6   //文件MD5校验错误
#define    ERR_NO_INIT	                7   //设备未初始化
#define    ERR_HARD_FAULT	            8   //设备硬件异常


#define    ERR_TERM_LINK_DOWN	        128   //终端掉线
#define    ERR_TERM_LINK_UP	            129   //终端掉线恢复
#define    ERR_TERM_VALUE_CHANGE	    130  //终端超阈值改变值
#define    ERR_TERM_VALUE_MAX_ON	    131  //终端超最大值
#define    ERR_TERM_VALUE_MAX_OFF	    132  //终端超最大值恢复
#define    ERR_TERM_VALUE_MIN_ON	    133  //终端超最小值
#define    ERR_TERM_VALUE_MIN_OFF	    134  //终端超最小值恢复


#define    STR_CODE             "code"              //string    	命令名称，具体定义见
#define    STR_SRC_TYPE         "src_type"          //int   	    标识数据来源，0：终端设备发送的数据；1：云平台发送的数据；2：第三方平台发送的数据    
#define    STR_DEV_ID           "dev_id"            //string    	管理末端设备的控制器/采集器的设备ID    
#define    STR_TIMESTAMP        "timestamp"         //int   	    时间戳，1970年秒数    
#define    STR_ENCRYPT          "encrypt"           //int   	    加密方式，具体定义见    
#define    STR_CONTENT          "content"           //JSON对象  	消息内容    
#define    STR_TERM_ID          "term_id"           //string    	末端设备ID     
#define    STR_CMD              "cmd"               //string数组    命令名，有多个命令时使用数组标识，每个数组元素对应一个命令，命令定义见
#define    STR_STATUS           "status"            //string数组    状态名，有多个状态时使用数组标识，每个数组元素对应一个状态，状态定义见    
#define    STR_VALUE            "value"             //double数组    命令/状态值，与命令名/状态名对应，有多个值时使用数组标识，每个数组元素对应一个值
#define    STR_FAULT            "fault"             //int数组   	设备故障状态，有多个故障时，使用数组标识，每个元素对应一个故障
#define    STR_TERM_NAME        "term_name"         //string    	终端名称    
#define    STR_FILE_PATH        "file_path"         //string    	配置文件存放的路径
#define    STR_PASSWORD         "password"          //string    	配置文件解压密码    
#define    STR_MD5              "md5"               //string    	配置文件md5校验码
#define    STR_RESULT           "result"            //int   	    配置文件下载结果，0为正常，其他值见错误代码    
#define    STR_TOTAL            "total"             //int   	    在线设备总数        
#define    STR_REMAIN           "remain"            //int   	    剩余未发送设备数量    
#define    STR_CONTROL_ID       "control_id"        //string数组    末端设备对应的控制器ID        
#define    STR_SLAVE_ID         "slave_id"          //int    	    末端设备地址                    
#define    STR_READ_COUNT       "read_count"        //int   	    读数据块的数量                        
#define    STR_WRITE_COUNT      "write_count"       //int       	写数据块的数量                
#define    STR_QUANTITY         "quantity"          //int   	    N/A
#define    STR_READ             "read"              //string   	    读数据块的key值
#define    STR_WRITE            "write"             //string      	写数据块的key值        
#define    STR_CONFIG           "config"            //string      	末端设备配置参数的key值        
#define    STR_POLLING_TIME     "polling_time"      //int           末端设备轮询时间                
#define    STR_MODBUS_SET       "modbus_set"        //int数组       末端设备modbus参数设置                
#define    STR_BAUDRATE         "baudrate"          //int          	波特率
#define    STR_WORDLENGTH       "wordlength"        //int  	        字长                
#define    STR_STOPBITS         "stopbits"          //int           停止位            
#define    STR_PARITY           "parity"            //int    	    奇偶校验位            
#define    STR_HWFLOWCTL        "hwflowctl"         //int    	    硬件流控            
#define    STR_REGISTERS        "registers"          //JSON对象数组   配置文件中下发的寄存器表格          
#define    STR_DATA             "data"              //int   	    配置文件中，控制命令的数据字段
#define    STR_FUNC_CODE        "func_code"         //int   	    modbus协议的功能码            
#define    STR_START_REG        "start_reg"         //int   	    modbus协议中寄存器起始地址            
#define    STR_REG_LEN          "reg_len"           //int           modbus协议中读取或控制的寄存器长度        
#define    STR_START_BIT        "start_bit"         //int   	    modbus协议中寄存器地址起始bit            
#define    STR_BIT_LEN          "bit_len"           //int   	    modbus协议中寄存器bit长度            
#define    STR_THRESHOLD_MIN    "threshold_min"     //int    	    告警最小阈值                                
#define    STR_THRESHOLD_MAX    "threshold_max"     //int           告警最大阈值                                
#define    STR_THRESHOLD_CHANGE "threshold_change"  //int           告警改变阈值                                    
#define    STR_SRC_ID           "src_id"            //string        发送方id                            
#define    STR_IP               "ip"                //string       	ip地址                    
#define    STR_PORT             "port"              //int       	port端口                        
#define    STR_BROKER           "broker"            //string        配置信息中，mqtt地址的key值                        
#define    STR_FILE_NAME        "file_name"         //string    	配置文件或者升级文件的文件名                        
#define    STR_FILE_TYPE        "file_type"         //string    	文件类型                        
#define    STR_FILE_SIZE        "file_size"         //int        	文件大小                        
#define    STR_DATA_SIZE        "data_size"         //int       	升级文件每一包的大小                   
#define    STR_TYPE             "type"              //string   	    OTA文件类型                
#define    STR_DEVICE_COUNT     "device_count"      //int   	    末端设备数量                            
#define    STR_RESPONSE         "response"          //int   	    应答key值                        
#define    STR_ERROR_CODE       "error_code"        //int           故障码                            
#define    STR_VERSION          "version"           //string        版本信息                            

#define    STR_ONLINE           "online"            //XTBOX下发的UDP消息类型
#define    STR_NET_CONF         "net_conf"          //XTBOX下发的UDP消息类型
#define    STR_DEV_ONLINE       "dev_online"        //XTBOX下发的UDP消息类型
#define    STR_ONLINE_ACK       "online_ack"        //XTBOX下发的UDP消息类型
#define    STR_NET_CONF_ACK     "net_conf_ack"      //XTBOX下发的UDP消息类型


#define sw16(A) ((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8))
#define sw32(A) ((((uint32_t)(A) & 0xff000000) >> 24) | (((uint32_t)(A) & 0x00ff0000) >> 8) | (((uint32_t)(A) & 0x0000ff00) << 8) | (((uint32_t)(A) & 0x000000ff) << 24))
#define sw64(A) ((uint64_t)(\
				(((uint64_t)(A)& (uint64_t)0x00000000000000ffULL) << 56) | \
				(((uint64_t)(A)& (uint64_t)0x000000000000ff00ULL) << 40) | \
				(((uint64_t)(A)& (uint64_t)0x0000000000ff0000ULL) << 24) | \
				(((uint64_t)(A)& (uint64_t)0x00000000ff000000ULL) << 8) | \
				(((uint64_t)(A)& (uint64_t)0x000000ff00000000ULL) >> 8) | \
				(((uint64_t)(A)& (uint64_t)0x0000ff0000000000ULL) >> 24) | \
				(((uint64_t)(A)& (uint64_t)0x00ff000000000000ULL) >> 40) | \
				(((uint64_t)(A)& (uint64_t)0xff00000000000000ULL) >> 56) ))

#define SW16(A)	sw16(((uint16_t)A))	
#define SW32(A)	sw32(((uint32_t)A))
#define SW64(A)	sw64(((uint64_t)A))	

//大端转小端，小端转大端，都用这个宏，无需判断类型，直接输入就可以输出对应类型的数据，无返回，直接转换
#define SWAP( type )   ( type = ( \
( sizeof(type)==1 ) ? (type) : ( \
( sizeof(type)==2 ) ? (SW16(type)) : ( \
( sizeof(type)==4 ) ? (SW32(type)) : ( \
( sizeof(type)==8 ) ? (SW64(type)) : ( \
type) ) ) ) ) )



/*函数声明*******************************************************************************************/

void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset);                         /* 设置中断偏移量 */
void sys_standby(void);                                                                     /* 进入待机模式 */
void sys_soft_reset(void);                                                                  /* 系统软复位 */
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);   /* 配置系统时钟 */


/* 以下为汇编函数 */
void sys_wfi_set(void);             /* 执行WFI指令 */
void sys_intx_disable(void);        /* 关闭所有中断 */
void sys_intx_enable(void);         /* 开启所有中断 */
void sys_msr_msp(uint32_t addr);    /* 设置栈顶地址 */

#endif

