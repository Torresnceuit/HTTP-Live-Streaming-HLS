#ifndef _G_FUNC_H
#define _G_FUNC_H

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string>

std::string ip_to_str(unsigned int ip);
void print_msg(std::string &strMsg, const char *msg, unsigned int len);
unsigned int ipport_2_flow(unsigned int uiIp, unsigned short usPort);
unsigned long long get_tick_count(timeval& curTVTime);

//获取eth0,eth1,lo网卡地址, ifname可选值: eth0,eth1,lo
unsigned int getipbyif(const char *ifname);

#endif
