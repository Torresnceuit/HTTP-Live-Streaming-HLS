#ifndef TFC_NET_CCD_DEFINE_H__
#define TFC_NET_CCD_DEFINE_H__

//发送ip 给 mq 的结构体
typedef struct
{
    unsigned int	_ip;
    unsigned short  _port;
    unsigned short	_type;
}TCCDHeader;

#define CCD_HEADER_LEN      8



//配置文件 send_event_2_mq = 0	 //0: not,  1: send ccd event to mq (ignore send_ip_2_mq)
//ccd 的命令： rsp(ccd->mq), req(mq->ccd)
enum ccd_reqrsp_type
{
	ccd_rsp_connect			= 101,	//有主机连接, enqueue_mq 的数据为 header
	ccd_rsp_disconnect		= 102,	//有主机断开连接, enqueue_mq 的数据为 header
	ccd_rsp_data			= 103,	//ccd 发数据来, enqueue_mq 的数据为 header + data
	ccd_rsp_send_ok			= 104,	//发送数据成功, enqueue_mq 的数据为 header
	ccd_rsp_send_nearly_ok	= 105,	//几乎发送数据成功(比如剩20k), enqueue_mq 的数据为 header
	ccd_rsp_error			= 106,	//有错误发生，原因不明，待查
	ccd_rsp_recv_fail		= 107,	//接收数据失败, enqueue_mq 的数据为 header
	ccd_rsp_send_fail		= 108,	//发送数据失败, enqueue_mq 的数据为 header
    ccd_rsp_timeout_close   = 120,  //ccd超时导致连接被关闭

    ccd_rsp_stat_conn_overload_count    = 300,	//拒绝连接统计, enqueue_mq 的数据为 header 或者 size=0 的包
    ccd_rsp_stat_grid_overload_count	= 301,	//格子检查关闭连接统计, enqueue_mq 的数据为 header 或者 size=0 的包

	ccd_req_disconnect		= 200,	//通知连接断裂, enqueue_mq 的数据为 header 或者 size=0 的包

    
};

//ccd 通知 mq 的事件
typedef struct
{
	unsigned int	_magic_num;		//ccd 消息标记 = 123123123
	unsigned int	_reqrsp_type;	//消息号
	unsigned int	_flow;			//主要标识
	unsigned int	_ip;			//消息的ip， port
	unsigned int	_port;
	unsigned int	_other;			//扩展用， 用来记录 send 延时， recv 延时,
}TCCDEventHeader;

#define CCD_EVENT_HEADER_LEN		24
#define CCD_EVENT_MAGIC_NUM			123123123



#endif
