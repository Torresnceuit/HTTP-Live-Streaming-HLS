#include <iostream>
#include "http_req.h"
#include "http_rsp.h"
#include "http_func.h"

using namespace std;
using namespace httpsvr;
using namespace httpsvr::func;

extern "C"
{
int req_complete_func(const void* data, unsigned data_len)
{
	if (data_len <= 0) 
		return 0;
	
	CHttpReq http_req(data, data_len);
    int ret = http_req.input(data, data_len);
    if (ret)
    { 
        fprintf(stderr, "h_req_complete ret: %d\n", ret);
        return -1;
    }
    
    if (http_req.inited()) 
        return (http_req._parsed_len);
    
    return 0;
}

int rsp_complete_func(const void* data, unsigned data_len)
{
    if (data_len <= 0) 
        return 0;
    
    CHttpRsp http_rsp;
    int ret = http_rsp.input(data, data_len);
    if (ret)
    {
        fprintf(stderr, "h_rsp_complete ret: %d, %s\n", ret, (char *)data);
        FILE *file = fopen("/data/dcc.out", "wb");
        fwrite(data, data_len, 1, file);
        fclose(file);
        return -1;
    }
    
    if (http_rsp.inited()) 
        return (http_rsp._parsed_len);
    
    return 0;
}

int net_complete_func(const void* data, unsigned data_len)
{
    //增加兼容性判断，如果发现前面是post, get, head开头的则使用req_complete_func来判断
    if (data_len <= 0) 
        return 0;
    
    //增加对探测请求量的包进行特殊处理   
    CHttpReq http_req(data, data_len);
    int ret = http_req.input(data, data_len);
    if (ret)
    { 
        return rsp_complete_func(data, data_len);
    }
    
    if (http_req.inited()) 
        return (http_req._parsed_len);
    
    return 0;
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
