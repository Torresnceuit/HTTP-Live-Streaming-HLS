#ifndef _SUBMCD_WRITER_PROC_H_
#define _SUBMCD_WRITER_PROC_H_

#include <sys/time.h>
#include <string>
#include <map>

#include "tfc_object.h"
#include "tfc_cache_proc.h"
#include "tfc_net_ipc_mq.h"
#include "tfc_net_conn_map.hpp"
#include "tfc_net_ccd_define.h"

#include "../lib/Attr_API.h"
#include "gFunc.h"

#include "muxsvr_submcd_writer_cfg.h"

using namespace std;
using std::map;

namespace muxsvr
{
	// 缓冲区大小
	const unsigned BUF_SIZE = 4*1024*1024;

	typedef struct FileHandle
	{
		int				iFd;

		unsigned int	uiWriteTime;
		unsigned int	uiFileSize;

		FileHandle()
		{
			iFd			= 0;
			uiWriteTime	= 0;
			uiFileSize	= 0;
		}

		FileHandle(int fd)
		{
			iFd			= fd;
			uiWriteTime = 0;
			uiFileSize	= 0;
		}

		FileHandle& operator = (const FileHandle& lsh)
		{
			iFd			= lsh.iFd;
			uiWriteTime	= lsh.uiWriteTime;
			uiFileSize	= lsh.uiFileSize;

			return *this;
		}

	}FileHandle;

	class FileHandleMgr
	{
		enum
		{
			MAX_WRITE_FILE_NUM		= 32,
			MAX_WRITE_FILE_TIMEOUT	= 60,
			MAX_WRITE_FILE_SIZE		= 10 * 1024 *1024,
		};

	public:
		FileHandleMgr(){}

		~FileHandleMgr()
		{
			map<unsigned int, FileHandle*>::iterator iter = writeFds.begin();

			for(;iter != writeFds.end(); iter++)
			{
				int fd = iter->second->iFd;

				if(fd)
				{
					close(fd);
					
					iter->second->iFd = 0;
				}
			}
		}

		//获取一个文件句柄
		FileHandle* getFileHandle(unsigned int uiVideoId)
		{
			map<unsigned int, FileHandle*>::iterator iter = writeFds.find(uiVideoId);

			//不存在，申请一个
			if(iter == writeFds.end())
			{
				return applyFileHandle(uiVideoId);
			}

			return iter->second;
		}

		//申请一个文件句柄
		FileHandle* applyFileHandle(unsigned int uiVideoId)
		{
			if(writeFds.size() > MAX_WRITE_FILE_NUM)
			{
				return NULL;
			}

			stringstream ss;
			ss << uiVideoId;

			string strName;
			ss >> strName;
			
			int fd = open(strName.c_str(), O_WRONLY);

			if(fd < 0)
			{
				return NULL;
			}

			FileHandle *pFileHandle = new FileHandle(fd);

			writeFds[uiVideoId] = pFileHandle;

			return pFileHandle;
		}

		unsigned int getFdSize() const
		{
			return writeFds.size();
		}

		//写文件
		int writeFile(unsigned char *pData, unsigned int uiLen, unsigned int uiVideoId)
		{
			if(NULL == pData || 0 == uiLen)
			{
				return -1;
			}

			FileHandle *pFileHandle	= getFileHandle(uiVideoId);

			if(NULL == pFileHandle)
			{
				return -2;
			}

			if(pFileHandle->uiFileSize > MAX_WRITE_FILE_SIZE)
			{
				return -3;
			}

			int iRet	= 0;
			int fd		= pFileHandle->iFd; 

			if(0 == fd)
			{
				return -4;
			}

			while(iRet < (int)uiLen)
			{
				iRet += write(fd, pData + iRet, uiLen - iRet);
			}

			pFileHandle->uiFileSize += uiLen;
			pFileHandle->uiWriteTime = time(NULL);

			return iRet;
		}

		//显式关闭文件
		int shutDownFile(unsigned int uiVideoId)
		{
			map<unsigned int, FileHandle*>::iterator iter = writeFds.find(uiVideoId);

			if(iter == writeFds.end())
			{
				return -1;
			}

			int fd = iter->second->iFd;

			close(fd);

			iter->second->iFd = 0;

			return fd;
		}

		int DoCleanTimeOutFileHandle()
		{
			unsigned int uiNumOfFFTimeout = 0;

			map<unsigned int, FileHandle*>::iterator it;

			for(it = writeFds.begin(); it != writeFds.end();)
			{
				if(time(NULL) - it->second->uiWriteTime > MAX_WRITE_FILE_TIMEOUT)
				{
					close(it->second->iFd);

					it->second->iFd = 0;

					delete it->second;
					writeFds.erase(it++);

					uiNumOfFFTimeout++;
				}
				else
				{
					it++;
				}
			}

			return uiNumOfFFTimeout;
		}

	private:
		map<unsigned int, FileHandle*> writeFds;
	};

	class CSubMcdWriterProc : public tfc::cache::CacheProc
	{
	public:
		CSubMcdWriterProc();
		virtual ~CSubMcdWriterProc();

		virtual void run(const std::string& conf_file);

		timeval m_curTVTime;
		time_t  m_cur_time;

		unsigned long long m_ll_now_Tick;

	public:
		void dispatch_mcd();

		FileHandleMgr m_oFileHandleMgr;

    private:
        time_t m_LastTimeDoCleanTimeOutFileHandle;

	protected:
		void dispatch_timeout();
		void init(const std::string &conf_file);

		void init_log();
		void init_ipc();

		int	 check_flags();

	protected:
		char* m_recv_buf;
		char* m_send_buf;

		CSubMcdWriterCfgMng m_oCfg;

		tfc::net::CFifoSyncMQ *_mq_mcd_2_submcd_writer;
	};
}

#endif

