#include <stdarg.h>
#include <stdio.h>

#include "submcd_log.h"

SubMcdDebugLog* SubMcdDebugLog::_instance = NULL;

void SubMcdDebugLog::log_p(int log_level, const char *FMT, ...)
{
    if (_mq_mcd_2_submcdlog == NULL || _m_cur_time == NULL)
    {
        return;
    }
    
    if (sizeof(_buf)<=sizeof(int))
    {
        return ;
    }
    
    *((int *)_buf) = log_level;

    va_list ap;
    va_start(ap, FMT);
    int offset = vsnprintf(_buf+sizeof(int), sizeof(_buf)-sizeof(int), FMT, ap);
    va_end(ap);

    unsigned int uiLogLen = 0;

    if (offset<0)
    {
        uiLogLen = sizeof(_buf);
    }
    else if ((unsigned int)offset>=(sizeof(_buf)-sizeof(int)))
    {
        uiLogLen = sizeof(_buf);
    }
    else
    {
        uiLogLen = sizeof(int) + offset;
    }

    _mq_mcd_2_submcdlog->enqueue(*_m_cur_time, _buf, uiLogLen, 0);
}

int SubMcdDebugLog::open(tfc::net::CFifoSyncMQ *mq_mcd_2_submcdlog, const time_t *m_cur_time)
{
    _mq_mcd_2_submcdlog = mq_mcd_2_submcdlog;
    _m_cur_time = m_cur_time;

    return 0;
}
