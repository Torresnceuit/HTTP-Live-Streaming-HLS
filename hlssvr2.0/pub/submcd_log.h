#ifndef _SUBMCD_DEBUG_LOG_H_
#define _SUBMCD_DEBUG_LOG_H_

#include <string.h>
#include "tfc_net_ipc_mq.h"

class SubMcdDebugLog
{
public:
    static SubMcdDebugLog *instance()
    {
        if (_instance == NULL)
        {
            _instance = new SubMcdDebugLog;
        }

        return _instance;
    }

    void log_p(int log_level, const char *FMT, ...);

    int open(tfc::net::CFifoSyncMQ *mq_mcd_2_submcdlog, const time_t *m_cur_time);

private:
    SubMcdDebugLog():_mq_mcd_2_submcdlog(NULL),_m_cur_time(NULL)
    {
        memset(_buf, 0, sizeof(_buf));
    }

private:
    static SubMcdDebugLog *_instance;
    char _buf[40480];
    tfc::net::CFifoSyncMQ *_mq_mcd_2_submcdlog;
    const time_t *_m_cur_time;
};

#define SUBMCD_DEBUG_P SubMcdDebugLog::instance()->log_p
#define SUBMCD_DEBUG_OPEN SubMcdDebugLog::instance()->open

#endif

