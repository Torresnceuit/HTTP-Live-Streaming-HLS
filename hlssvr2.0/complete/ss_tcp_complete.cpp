#include <arpa/inet.h>
#include "http_req.h"
#include "http_rsp.h"
#include "http_func.h"

using namespace httpsvr;
using namespace httpsvr::func;

extern "C"
{

//     tcp��������Ҫ������ȡ���ϡ���Ȩ�ͶԷ��䡢��Ա�Ĳ�����Э���ʽ���£�
//     PackIn	= wPkgLen + 0x2 + cld_pkg_head + wStrGTKey(sBody) + 0x3
//     PackOut	= wPkgLen + 0x2 + svr_pkg_head + wStrGTKey(sBody) + 0x3
//     ע�⣺wPkgLenΪ���������ȣ�sBodyΪ������,��ʹ��GTKey���д������
//     GTKey����QQǩ����ÿ��tcp���Ӻ��һ�������֤���д���QQǩ��
//     QQǩ��ʹ��ST_Key�⿪��ST_Keyͨ����Ȩ���Ļ�ȡ
// 
//     tcp��������Ҫ�������֡�����hello���㲥��Э���ʽ���£�
//     PackIn	= wPkgLen + 0x2 + cld_pkg_head + wStrGTKey(sBody) + 0x3
//     PackOut	= wPkgLen + 0x2 + svr_pkg_head + wStrGTKey(sBody) + 0x3
//     ע�⣺wPkgLenΪ���������ȣ�sBodyΪ������,��ʹ��GTKey���д������
//     GTKey����QQǩ����ÿ��tcp���Ӻ��һ�������֤���д���QQǩ��
//     QQǩ��ʹ��ST_Key�⿪��ST_Keyͨ����Ȩ���Ļ�ȡ

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
		unsigned int    uiLength;           // �����ȣ����������ֶ�(����ClientHeader)
		unsigned int    uiUin;              // �û�UIN
		unsigned short  ushVersion;         // Э��汾�ţ� qt��0
		unsigned int    uiAppID;            // Ӧ��ID��     qt��0
		unsigned int    uiZoneID;           // ����ID��     qt��0
		unsigned short  ushCmdID;           // ������
		unsigned int    uiDestSvrID;        // Ŀ�ķ��������ͣ�qt��0
		unsigned int    uiRequest;          // �ͻ������к�,   qt��0
		unsigned short  ushCheckSum;        // ����У��,       qt��0

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

	//gamesvr�ذ�
	ClientHeader *pClientHeader = (ClientHeader*)data;

	if(ntohs(pClientHeader->ushCmdID) == 26004)
	{
		unsigned int   uiPkgLen= ntohl(*(unsigned int*)(data));

		if(data_len >= uiPkgLen)
		{
			return uiPkgLen;
		}
	}
	//http����
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
            //����
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

//netdata_len��Ҫ�����
//return -1ʧ�ܣ�0�ɹ���1δ�ı�(����memcpy����)
int protocol_ipc2net(const void* ipcdata, unsigned ipcdata_len,void* netdata, unsigned &netdata_len)
{
    return 1;
}

//ipcdata_len��Ҫ�����
//return -1ʧ�ܣ�0�ɹ���1δ�ı�(����memcpy����)
int protocol_net2ipc(const void* netdata, unsigned netdata_len,void* ipcdata, unsigned &ipcdata_len)
{
    return 1;
}

}
