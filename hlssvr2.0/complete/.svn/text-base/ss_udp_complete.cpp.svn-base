extern "C"
{

//     在server交互中：
//     所有数据传输都采用udp，使用内部统一的国际化包格式：
//     Pack = 0xa + DBPkgHead + RelayPkgHeadEx2 + cld_pkg_head + sBody + 0x3

typedef struct
{
    char sLength[2];
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

typedef struct
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

int req_complete_func(const void* data, unsigned data_len)
{  
	if (data_len <= 0) 
    {
		return 0;
    }

    const char *p = (char *)data;
    
    if ((p[0] != 0xa)
        ||(p[data_len-1] != 0x03))
    {
        return -1;
    }

    if (data_len < sizeof(char) + sizeof(DBPkgHead) + sizeof(RelayPkgHeadEx2) + sizeof(cld_pkg_head) + sizeof(char))
    {
        return -1;
    }
    else
    {
        return data_len;
    }       
}

int rsp_complete_func(const void* data, unsigned data_len)
{
    if (data_len <= 0) 
    {
        return 0;
    }

    const char *p = (char *)data;

    if ((p[0] != 0xa)
        ||(p[data_len-1] != 0x03))
    {
        return -1;
    }

    if (data_len < sizeof(char) + sizeof(DBPkgHead) + sizeof(RelayPkgHeadEx2) + sizeof(cld_pkg_head) + sizeof(char))
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
