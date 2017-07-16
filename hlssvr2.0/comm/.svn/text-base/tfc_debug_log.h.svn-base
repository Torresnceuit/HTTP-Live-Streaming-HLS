#ifndef _TFC_DEBUG_LOG_H_
#define _TFC_DEBUG_LOG_H_

#include "tfc_comm.h"
#ifndef WIN32
#include "tfc_thread_sync.h"
#endif

/*
日志类.

*/

const int DEFAULT_MAX_FILE_SIZE = (1024*1024*1024);     //最大文件SIZE为1G
const int DEFAULT_MAX_FILE_NO = 1;                        //默认最大文件编号
const int MAX_LOG_NAME_LEN = 1024;

enum LOG_TYPE
{
    LOG_TYPE_NORMAL = 0,
    LOG_TYPE_CYCLE = 1,
    LOG_TYPE_DAILY,
    LOG_TYPE_HOURLY,
};
enum LOG_LEVEL
{
    LOG_TRACE = 1,
    LOG_DEBUG = 2,
    LOG_NORMAL= 4,
    LOG_ERROR = 8,
    LOG_FATAL = 16,
    LOG_VITAL = 100,
	LOG_COLOR = 1001,
    LOG_NONE  = 10000,     //用于设置不打印任何日志
};

typedef struct tagLogPara
{
	int log_level_;
	int log_type_;
	string path_;
	string name_prefix_;
	int max_file_size_;
	int max_file_no_;	
}TLogPara;

class TFCDebugLog
{
protected:
    bool file_open_;
    int log_level_;
    int log_type_;
    FILE  *file_;
    int max_file_size_;
    int max_file_no_;      //最多文件个数, 仅当log_type_ == LOG_TYPE_NORMAL时有效

    int cur_file_size_;
    int cur_file_no_;

    int pre_time_;
    string path_;
    string name_prefix_;
    string filename_;

#ifndef WIN32
    CMutex p_mutex_;
#endif
    static TFCDebugLog* instance_;
public:
     //调试信息

    TFCDebugLog();
    ~TFCDebugLog();

    static TFCDebugLog* instance();
    static int init();
    static void fini();
     
    static char* GetPackTimeFormat(int iTime, char* szDateTime);

    static char* GetPackCurTimeFormat(char* szDateTime);

    static char* GetTimeFormat(int iTime, char* szDateTime);

    static char* GetCurTimeFormat(char* szDateTime);

    int open(int log_level, int log_type=LOG_TYPE_NORMAL, char* path=NULL, char* name_prefix=NULL, 
                int max_file_size=DEFAULT_MAX_FILE_SIZE, int max_file_no=DEFAULT_MAX_FILE_NO);
    int open(int log_level, int log_type, string path, string name_prefix, 
                int max_file_size=DEFAULT_MAX_FILE_SIZE, int max_file_no=DEFAULT_MAX_FILE_NO);
    void log_level(int level);
///
	int log_level(void);

    void log_p(int log_level, const char* FMT, ...);
    void log_p_no_time(int log_level, const char* FMT, ...);
	void log_buf_p(int log_level, const char *buf, int iLen);
	void log_bin_p(int log_level, const char *buf, int iLen);

    void flush_file(void);

     void close_file();
protected:
    int force_rename(const char* src_pathname, const char* dest_pathname);
    int init_cur_file_no();
    int build_file_name(char* pathname=NULL, string* filename=NULL);
    int open_file();
    int shift_file(int cur_time);

    void log_i(int log_level, int* pt, const char* FMT, ...);    //内部LOG函数
    void log_i_v(int log_level, int* pt, const char* FMT, va_list ap);    //内部LOG函数
    void log_buf_i(int log_level, int *pt, const char *buf, int iLen); //内部LOG函数
};


#define DEBUG_P TFCDebugLog::instance()->log_p
#define DEBUG_P_NO_TIME TFCDebugLog::instance()->log_p_no_time
#define DEBUG_BUF_P TFCDebugLog::instance()->log_buf_p
#define DEBUG_OPEN TFCDebugLog::instance()->open
#define DEBUG_INIT TFCDebugLog::init
#define DEBUG_FINI TFCDebugLog::fini

#define DEBUG_SET_LEVEL TFCDebugLog::instance()->log_level

#endif //_TFC_DEBUG_LOG_H_

