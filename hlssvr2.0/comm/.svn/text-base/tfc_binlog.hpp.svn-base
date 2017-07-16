#ifndef _TFC_BINLOG
#define _TFC_BINLOG

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>

#define MAX_BINLOG_ITEM_LEN	(32*1024*1024+512)

class CBinLog
{
public:
	CBinLog();
	~CBinLog();
	
	int Init(char *sPLogBaseName, long lPMaxLogSize, int iPMaxLogNum);
	int SetReadRecordStartTime(int starttime = -1);
	int ClearAllBinLog();
	int WriteToBinLog(char* buff, int len);
	int ReadRecordFromBinLog(char* buff, int maxsize);
	int ShiftFiles();

private:
	char *szTmpBuff;
	char _log_basename[256];
	long _maxlog_size;
	int _maxlog_num;

	FILE *_write_fp;
	
	FILE *_read_fp;	
	int _readidx;
};

#endif

