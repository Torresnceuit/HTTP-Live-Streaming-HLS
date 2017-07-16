#include <arpa/inet.h>
#include "http_req.h"
#include "http_rsp.h"
#include "http_func.h"

using namespace httpsvr;
using namespace httpsvr::func;

extern "C"
{

//     tcp短连接主要用于拉取资料、鉴权和对房间、成员的操作，协议格式如下：
//     PackIn	= wPkgLen + 0x2 + cld_pkg_head + wStrGTKey(sBody) + 0x3
//     PackOut	= wPkgLen + 0x2 + svr_pkg_head + wStrGTKey(sBody) + 0x3
//     注意：wPkgLen为整个包长度，sBody为不定长,且使用GTKey进行传输加密
//     GTKey来自QQ签名，每次tcp连接后第一个身份验证包中带有QQ签名
//     QQ签名使用ST_Key解开，ST_Key通过鉴权中心获取
// 
//     tcp长链接主要用于文字、文字hello、广播，协议格式如下：
//     PackIn	= wPkgLen + 0x2 + cld_pkg_head + wStrGTKey(sBody) + 0x3
//     PackOut	= wPkgLen + 0x2 + svr_pkg_head + wStrGTKey(sBody) + 0x3
//     注意：wPkgLen为整个包长度，sBody为不定长,且使用GTKey进行传输加密
//     GTKey来自QQ签名，每次tcp连接后第一个身份验证包中带有QQ签名
//     QQ签名使用ST_Key解开，ST_Key通过鉴权中心获取

#pragma pack(1)
	typedef struct DBPkgHead
	{
		unsigned short usLen;
		unsigned char cCommand;
		char sServerID[2];
		char sClientAddr[4];
		char sClientPort[2];
		char sConnAddr[4];
		char sConnPort[2];
		char sInterfaceAddr[4];
		char sInterfacePort[2];
		char cProcessSeq;
		unsigned char cDbID;
		char sPad[2];               // Pad the same length as RelayPkgHead.
	} DBPkgHead;

	typedef struct RelayPkgHeadEx2
	{
		unsigned short shExVer;
		unsigned short shExLen;
		unsigned short shLocaleID;
		short shTimeZoneOffsetMin;
		char sReserved[0];
	} RelayPkgHeadEx2;

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

	typedef struct ClientHeader
	{
		unsigned int    uiLength;           // 包长度，包括数据字段(包括ClientHeader)
		unsigned int    uiUin;              // 用户UIN
		unsigned short  ushVersion;         // 协议版本号， qt填0
		unsigned int    uiAppID;            // 应用ID，     qt填0
		unsigned int    uiZoneID;           // 大区ID，     qt填0
		unsigned short  ushCmdID;           // 命令字
		unsigned int    uiDestSvrID;        // 目的服务器类型，qt填0
		unsigned int    uiRequest;          // 客户端序列号,   qt填0
		unsigned short  ushCheckSum;        // 数据校验,       qt填0

		ClientHeader()
		{
			uiLength	= 0;
			uiUin		= 0;
			ushVersion	= 0;
			uiAppID		= 0;
			uiZoneID	= 0;
			ushCmdID	= 0;
			uiDestSvrID	= 0;
			uiRequest	= 0;
			ushCheckSum	= 0;
		}

	}ClientHeader;
#pragma pack()

int req_complete_func(const void* data, unsigned data_len)
{  
	if (data_len <= 2) 
    {
		return 0;
    }

    const char *p = (char *)data;
        
    unsigned short wPkgLen = ntohs(*(unsigned short*)(p));

	//gamesvr回包
	ClientHeader *pClientHeader = (ClientHeader*)data;

	if(ntohs(pClientHeader->ushCmdID) == 26004)
	{
		unsigned int   uiPkgLen= ntohl(*(unsigned int*)(data));

		if(data_len >= uiPkgLen)
		{
			return uiPkgLen;
		}
	}
	//http请求
	else
	{
		CHttpReq http_req(data, data_len);
		int ret = http_req.input(data, data_len);
		if (ret)
		{ 
			fprintf(stderr, "h_req_complete ret: %d\n", ret);
			return -1;
		}

		if (http_req.inited()) 
		{
			return (http_req._parsed_len);
		}
	}

	return 0;
}

int rsp_complete_func(const void* data, unsigned data_len)
{
    if (data_len <= 2) 
    {
        return 0;
    }

    const char *p = (char *)data;

    unsigned short wPkgLen = ntohs(*(unsigned short*)(p));

    if ((p[2] != 0x0a))
    {
        return -1;
    }

//     if (wPkgLen < sizeof(unsigned short) + sizeof(char) + sizeof(svr_pkg_head) + sizeof(char) + 1)
//     {
//         return -1;
//     }

    if (data_len >= wPkgLen)
    {
        if (p[wPkgLen -1] != 0x03)
        {
            return -1;
        }
        else
        {
            //隐患
            if (wPkgLen > 1024)
            {
                return -1;
            }
            else
            {
                return wPkgLen;
            }
        }       
    }
    else
    {
        return 0;
    }  
}

int net_complete_func(const void* data, unsigned data_len)
{
    if (data_len <= 0) 
    {
        return 0;   
    }

	return req_complete_func(data, data_len);
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
