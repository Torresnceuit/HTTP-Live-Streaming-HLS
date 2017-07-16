/*
公共的系统头文件包含
公共的常量, 宏定义
*/

#ifndef _TFC_COMM_H_
#define _TFC_COMM_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <stdarg.h>

#ifndef WIN32
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/shm.h>
//#include <sys/epoll.h>
#include <cstdio>
#include <sys/mman.h>
#include <dirent.h>

#else
#include "winsock2.h"
#include <conio.h>
#endif

#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <set>

//#include "mysql.h"

using namespace std;


//错误值定义
const int TFC_MAX_ERROR_MSG_LEN = 1024;
const int TFC_SUCCESS = 0;
/*
TFC_ERROR 为0 时表示成功, < 0 时表示失败. 具体错误值按各类的定义
*/



//
#ifdef WIN32
#define vsnprintf _vsnprintf
#define snprintf _snprintf

const char DIR_SEPERATOR ='\\';
#else

const char DIR_SEPERATOR ='/';
#endif

#endif //_TFC_COMM_H_

