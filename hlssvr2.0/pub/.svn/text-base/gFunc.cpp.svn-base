#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/time.h>
#include <string>

#include "gFunc.h"

using namespace std;

string ip_to_str(unsigned int ip)
{
    struct in_addr inaddr;
    inaddr.s_addr=ip;
    return string(inet_ntoa(inaddr));
}

void print_msg(string &strMsg, const char *msg, unsigned int len)
{
    char tmp[4]={0};

    for(unsigned int i=0; i<len; i++) 
    {
        snprintf(tmp, sizeof(tmp), "%02X ", (char)msg[i]);
        strMsg.append(tmp);

        if ((i+1)%8 == 0)
        {
            strMsg.append(" ");
        }

        if ((i+1)%16 == 0)
        {
            strMsg.append("\n");
        }
    }
}

unsigned int ipport_2_flow(unsigned int uiIp, unsigned short usPort)
{
    static const unsigned int UI_MAX_FLOWSIZE = 1024*1024 - 1;
    unsigned int uiFlow = (uiIp>>16) + usPort;
    if (uiFlow%UI_MAX_FLOWSIZE == 0)
    {
        uiFlow = uiFlow + 1;
    }
    uiFlow = uiFlow%UI_MAX_FLOWSIZE;

    return uiFlow;
}

unsigned long long get_tick_count(timeval& curTVTime)
{
    gettimeofday(&curTVTime, NULL); 
    return (unsigned long long)curTVTime.tv_sec*1000 + curTVTime.tv_usec/1000;
}

//获取eth0,eth1,lo网卡地址, ifname可选值: eth0,eth1,lo
unsigned int getipbyif(const char *ifname) 
{
    if(!ifname)
    {
        return 0;
    }

    register int fd, intrface;
    struct ifreq buf[10];
    struct ifconf ifc;
    unsigned ip = 0;  

    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {   
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;

        if(!ioctl(fd, SIOCGIFCONF, (char*)&ifc))
        {
            /*lint !e414*/ 
            intrface = ifc.ifc_len / sizeof(struct ifreq);

            while(intrface-- > 0)  
            {           
                if(strcmp(buf[intrface].ifr_name, ifname) == 0)
                {           
                    if(!(ioctl(fd, SIOCGIFADDR, (char *)&buf[intrface])))
                    {
                        ip = (unsigned int)((struct sockaddr_in *)(&buf[intrface].ifr_addr))->sin_addr.s_addr;
                    }

                    break;  
                }    
            }
        }

        close(fd);
    }

    return ip;  
}
