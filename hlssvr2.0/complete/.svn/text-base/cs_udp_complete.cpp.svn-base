#include "stdio.h"

extern "C"
{

//     udp连接主要用于语音、语音hello、语音重传、语音EOS，协议格式如下：
//     语音：
//     PackIn	= 0x2+cld_pkg_head+cSubcmd+wStrVoiceConnKey(sBody)+cStrVoiceData+0x3
//     PackOut	= 0x2+svr_pkg_head+cSubcmd+wStrVoiceConnKey(sBody)+cStrVoiceData+ ... +0x3
//     语音hello、重传、EOS、检测：
//     PackIn	= 0x2+cld_pkg_head+cSubcmd+wStrVoiceConnKey(sBody)+0x3
//     PackOut	= 0x2+svr_pkg_head+cSubcmd+wStrVoiceConnKey(sBody)+0x3
//     注意：VoiceConnKey来自进房签名，在申请进入房间时由房间服务器派发
//     cStrVoiceData为语音数据包，不加密。
//     除了语音，其他所有信息都包含在sBody中，包括文字数据。
//     信息消息体最大长度控制在800字节以内

typedef struct
{
    char version[2];
    char command[2];
    char seq_num[2];
    char uin[4];
} cld_pkg_head;

typedef struct
{
    char version[2];
    char command[2];
    char seq_num[2];
} svr_pkg_head;

int req_complete_func(const void* data, unsigned data_len)
{  
	if (data_len <= 0) 
    {
		return 0;
    }

    const char *p = (char *)data;
    
    if ((p[0] != 0x2)
        ||(p[data_len-1] != 0x03))
    {
		fprintf(stderr, "cs_udp_msg not complete\n");
        return -1;
    }

	//fprintf(stderr, "cs_udp_msg complete\n");

    if (data_len < sizeof(char) + sizeof(cld_pkg_head) + sizeof(unsigned char) + sizeof(char) + 1)
    {
        return -1;
    }
    else
    {
        if (data_len > 1024)
        {
			fprintf(stderr, "cs_udp_msg exceed 1024 bytes\n");
            return -1;
        }
        else
        {
            return data_len;
        }
    }       
}

int rsp_complete_func(const void* data, unsigned data_len)
{
    if (data_len <= 0) 
    {
        return 0;
    }

    const char *p = (char *)data;

    if ((p[0] != 0x2)
        ||(p[data_len-1] != 0x03))
    {
        return -1;
    }

    if (data_len < sizeof(char) + sizeof(svr_pkg_head) + sizeof(unsigned char) + sizeof(char) + 1)
    {
        return -1;
    }
    else
    {
        return data_len;
    }  
}

int net_complete_func(const void* data, unsigned data_len)
{
    //增加兼容性判断，如果发现前面是post, get, head开头的则使用req_complete_func来判断
    if (data_len <= 0) 
    {
        return 0;   
    }

	return data_len;
}

//netdata_len需要被检查
//return -1失败，0成功，1未改变(避免memcpy消耗)
int protocol_ipc2net(const void* ipcdata, unsigned ipcdata_len,void* netdata, unsigned &netdata_len)
{
    return 1;
}

//ipcdata_len需要被检查
//return -1失败，0成功，1未改变(避免memcpy消耗)
int protocol_net2ipc(const void* netdata, unsigned netdata_len,void* ipcdata, unsigned &ipcdata_len)
{
    return 1;
}

}
