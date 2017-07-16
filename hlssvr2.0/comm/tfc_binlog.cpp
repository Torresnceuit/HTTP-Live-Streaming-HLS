#include "tfc_binlog.hpp"

CBinLog::CBinLog()
{
	_maxlog_size = 20000000;
	_maxlog_num = 8;
	strncpy(_log_basename,"./mcdbin_", sizeof(_log_basename)-1);
	_write_fp = NULL;
	
	_read_fp = NULL;
	_readidx = _maxlog_num;

	szTmpBuff = new char[MAX_BINLOG_ITEM_LEN];
}

CBinLog::~CBinLog()
{
	if (_write_fp) 
	{
		fclose(_write_fp);
		_write_fp = NULL;
	}	
	if (_read_fp) 
	{
		fclose(_read_fp);
		_read_fp = NULL;
	}	
	delete szTmpBuff;
}

int CBinLog::Init(char *sPLogBaseName, long lPMaxLogSize, int iPMaxLogNum)
{
	if (_write_fp) 
	{
		fclose(_write_fp);
		_write_fp = NULL;
	}	
	if (_read_fp) 
	{
		fclose(_read_fp);
		_read_fp = NULL;
	}
	
	memset(_log_basename, 0, sizeof(_log_basename));
	strncpy(_log_basename, sPLogBaseName, sizeof(_log_basename)-1);

	_maxlog_num = iPMaxLogNum;
	_maxlog_size = lPMaxLogSize;	
	return 0;
}

int CBinLog::ClearAllBinLog()
{
	char sLogFileName[300];
    memset(sLogFileName, 0, 300);
	for (int i=_maxlog_num-1; i>=0; i--)
	{
	        if (i == 0)
	            snprintf(sLogFileName, 299, "%s.log", _log_basename);
	        else
	            snprintf(sLogFileName, 299, "%s%d.log", _log_basename, i);
		
		if (access(sLogFileName, F_OK) != 0)
		{
			continue;
		}
		remove(sLogFileName) ;
	}

	if (_write_fp)
	{
		fclose(_write_fp);
		_write_fp = NULL;	
	}
	if (_read_fp)
	{
		fclose(_read_fp);
		_read_fp = NULL;	
	}	
	return 0;
}

int CBinLog::SetReadRecordStartTime(int starttime/*=-1*/)
{
	if (_read_fp)
	{
		fclose(_read_fp);
		_read_fp = NULL;
	}

	if (starttime == -1)
	{
		_readidx = _maxlog_num;
		return 0;
	}
	
	char sLogFileName[300];
    memset(sLogFileName, 0, 300);
	int tLogTime;
	int len;
	char buff[65535];
	FILE *_read_fp_mybe = NULL;
	int maybeidx = 0;
	
	for (_readidx = 0; _readidx < _maxlog_num; _readidx++)
	{
		if (_readidx == 0)
			snprintf(sLogFileName, 299, "%s.log", _log_basename);
		else
			snprintf(sLogFileName, 299, "%s%d.log", _log_basename, _readidx);

		if(access(sLogFileName, F_OK) != 0)
		{
			continue;
		}
		
		_read_fp = fopen(sLogFileName,"r+");	
		if (!_read_fp)
		{
			return -1;
		}
		fread(&tLogTime,sizeof(int),1,_read_fp);
		
		//必定在本文件中
		if (tLogTime < starttime)
		{
			fread(&len,sizeof(int),1,_read_fp);
			if (len > (int)sizeof(buff))
			{
				fclose(_read_fp); 
				_read_fp = NULL;
				return -1;
			}
			fread(buff,len,1,_read_fp);

			//后续判断
			do
			{
				fread(&tLogTime,sizeof(int),1,_read_fp);
				fread(&len,sizeof(int),1,_read_fp);
				if (len > (int)sizeof(buff))
				{
					fclose(_read_fp); 
					_read_fp = NULL;				
					return -1;
				}				
				fread(buff,len,1,_read_fp);
			}while((tLogTime<starttime) && (!feof(_read_fp)));

			//找到了
			if (tLogTime >= starttime)
			{
				//字节回退
				fseek(_read_fp,-8-len,SEEK_CUR);
				break;
			}

			if (feof(_read_fp))
			{
				fclose(_read_fp);
				_read_fp = NULL;
				if (_read_fp_mybe)
				{
					_read_fp = _read_fp_mybe;				
					_readidx = maybeidx;
				}
			}

			break;
		}	
		//必定不在本文件中
		else if (tLogTime > starttime)
		{
			fclose(_read_fp);
			_read_fp = NULL;
		}
		//可能在本文件中有一部分		
		else
		{
			if (_read_fp_mybe) fclose(_read_fp_mybe);

			//回退4字节
			fseek(_read_fp,-4,SEEK_CUR);
			_read_fp_mybe = _read_fp;
			_read_fp = NULL;
			maybeidx = _readidx;
		}
	}

	if (_read_fp)
	{
		return 0;
	}
	else if(_read_fp_mybe)
	{
		_read_fp = _read_fp_mybe;
		_readidx = maybeidx;
		return 0;
	}
	
	return -1;

}

//返回长度<0时结束,=0请忽略并继续
int CBinLog::ReadRecordFromBinLog(char* buff, int maxsize)
{
	char sLogFileName[300];	
    memset(sLogFileName, 0, 300);
	int tLogTime = 0;
	int len = 0;

	//定位下一个_read_fp
	if(!_read_fp || feof(_read_fp))
	{
		if (_read_fp)
		{
			fclose(_read_fp);
			_read_fp = NULL;
		}
		
		//找下一个文件
		do
		{
			_readidx--;
			if (_readidx == 0)
				snprintf(sLogFileName, 299, "%s.log", _log_basename);
			else
				snprintf(sLogFileName, 299, "%s%d.log", _log_basename, _readidx);
		}while((_readidx>=0) && (access(sLogFileName, F_OK)!=0));

		//读完了
		if (_readidx < 0)
		{
			return -1;
		}
		_read_fp = fopen(sLogFileName,"r+");		
	}
	
	if (!_read_fp)
	{
		return -2;
	}

	//只有read一次才知道feof...
	fread(&tLogTime,sizeof(int),1,_read_fp);
	fread(&len,sizeof(int),1,_read_fp);

	if (len > maxsize)
	{
		fclose(_read_fp);
		_read_fp = NULL;
		return -3;
	}
	if (feof(_read_fp))
	{
		//本文件结束,继续下一个文件
		return ReadRecordFromBinLog(buff,maxsize);
	}
	
	int ret = fread(buff,len,1,_read_fp);	
	return ret;
}

int CBinLog::WriteToBinLog(char* buff, int len)
{
	if (!buff || len<=0 || len > MAX_BINLOG_ITEM_LEN)
	{
		return -1;
	}
	
    char sLogFileName[300];
    memset(sLogFileName, 0, 300);
  	snprintf(sLogFileName, 299, "%s.log", _log_basename);
	if (_write_fp)
	{
		if (access(sLogFileName, F_OK) != 0)
		{
			fclose(_write_fp);
			_write_fp = NULL;
		}	
	}
			
	if (!_write_fp)
	{	    
		_write_fp = fopen(sLogFileName,"a+");
		if (!_write_fp)
		{
			return -1;
		}
	}

	int tNow = time(NULL);
	
	memcpy(szTmpBuff,&tNow,sizeof(int));
	memcpy(szTmpBuff+sizeof(int),&len,sizeof(int));
	memcpy(szTmpBuff+sizeof(int)+sizeof(int),buff,len);
	
	int iret = fwrite(szTmpBuff,len+sizeof(int)+sizeof(int),1,_write_fp);
	if (iret != 1)
	{
		return -11;
	}

#ifndef _NO_SYNC_
	iret = fflush(_write_fp);
	if (iret != 0)
	{
		return -12;
	}
	
#endif	

	return ShiftFiles();
}

int CBinLog::ShiftFiles()
{
   struct stat stStat;
    char sLogFileName[300];
    memset(sLogFileName, 0, 300);
    char sNewLogFileName[300];
    memset(sNewLogFileName, 0, 300);
    int i;

	snprintf(sLogFileName, 299, "%s.log", _log_basename);
    if(stat(sLogFileName, &stStat) < 0)
    {
        return -1;
    }

    if (stStat.st_size < _maxlog_size)
    {
        return 0;
    }

	if (_write_fp) 
	{
		fclose(_write_fp);
		_write_fp = NULL;
	}	
	
	//last file delete
    snprintf(sLogFileName, 299, "%s%d.log", _log_basename, _maxlog_num-1);
    if (access(sLogFileName, F_OK) == 0)
    {
        if (remove(sLogFileName) < 0 )
        {
            return -1;
        }
    }

    for(i = _maxlog_num-2; i >= 0; i--)
    {
        if (i == 0)
            snprintf(sLogFileName, 299, "%s.log", _log_basename);
        else
            snprintf(sLogFileName, 299, "%s%d.log", _log_basename, i);
            
        if (access(sLogFileName, F_OK) == 0)
        {
            snprintf(sNewLogFileName, 299, "%s%d.log", _log_basename, i+1);
            if (rename(sLogFileName,sNewLogFileName) < 0 )
            {
                return -1;
            }
        }
    }
    return 0;
}

