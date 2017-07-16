#ifndef _MUXSVR_H_
#define _MUXSVR_H_

#include <string.h>
#include <string>
#include <sstream>
#include <bitset>
#include <map>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace std;
using std::bitset;
using std::map;

namespace muxsvr 
{
#pragma pack(1)

    const unsigned int SOCKET_UDP_80    = 1;
    const unsigned int SOCKET_UDP_8000  = 2;
    const unsigned int SOCKET_UDP_443   = 3;

    const unsigned int SOCKET_TCP_80    = 4;
    const unsigned int SOCKET_TCP_8000  = 5;
    const unsigned int SOCKET_TCP_443   = 6;

    const unsigned int SRCTYPE_DCC      = 100;
    const unsigned int SRCTYPE_DCC2     = 101;
	
    const unsigned int MAX_CDN_FLOW_NUM         = 100;

	const unsigned int MAX_DIS_ARRAY_NUM		= 20;

    const unsigned int FRAME_ITEM_NUM           = 256;
    const unsigned int G_UIFRAMEDATABUFFSZIE    = 4*1024*1024;
	const unsigned int G_UIFLVCIRCULARSIZE      = 4*1024*1024;

	const unsigned int AUDIO_ITEM_NUM			= 4096;

	const unsigned int MAX_FRAME_LEN	= 1024*512;

	const int MAX_VIDEOFRAMEBUFFER_NUMBER = FRAME_ITEM_NUM;

	const unsigned int MAX_FLV_HEAD_LEN	= 8196;
    const unsigned int MAX_BLOCK_LEN = 1024*1024;

	#define CMD_VOICE	0x184

	enum
	{
		SUBMCD_OPEN		= 0x01,
		SUBMCD_WRITE	= 0x02,
		SUBMCD_CLOSE	= 0x03,
	};

    typedef struct cld_pkg_head
    {
        unsigned short version;
        unsigned short command;
        unsigned short seq_num;
        unsigned int uin;

		cld_pkg_head()
		{
			 memset(this, 0, sizeof(cld_pkg_head));
		}
    } cld_pkg_head;

    typedef struct DBPkgHead
    {
        unsigned short usLen;
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

        DBPkgHead()
        {
            memset(this, 0, sizeof(DBPkgHead));
        }

        void clear()
        {
            memset(this, 0, sizeof(DBPkgHead));
        }
    } DBPkgHead;

    typedef struct RelayPkgHeadEx2
    {
        unsigned short shExVer;
        unsigned short shExLen;
        unsigned short shLocaleID;
        short shTimeZoneOffsetMin;
        char sReserved[0];
        RelayPkgHeadEx2()
        {
            memset(this, 0, sizeof(RelayPkgHeadEx2));
        }

        void clear()
        {
            memset(this, 0, sizeof(RelayPkgHeadEx2));
        }
    } RelayPkgHeadEx2;

    //  ucType类型：
    //  HT_HEADER = 1（flv协议头），HT_DATA = 2（数据帧）
    //  ucKeyFrame类型：:
    //  TYPE_NORMAL_FRAME = 1(普通帧), TYPE_KEY_FRAME = 2(关键帧)
    enum
    {
        HT_HEADER   = 1,            //flv协议头
        HT_DATA     = 2,            //数据帧
    };

    enum
    {
        TYPE_NORMAL_FRAME   = 1,    //普通帧
        TYPE_KEY_FRAME      = 2,    //关键帧
    };

    typedef struct tagHEADBASE
    {
        unsigned int   uSize;           //大小(数据部分大小) 
        unsigned short huHeaderSize;    //本数据结构头的大小
        unsigned short huVersion;       //版本号   
    }tagFLVHEADBASE;

    typedef struct tagFLVHEAD : public tagHEADBASE 
    {
        unsigned char  ucType;          //类型
        unsigned char  ucKeyFrame;      //标识是不是关键帧
        unsigned int   uSec;            //时间戳   时间(秒) 用于回看的
        unsigned int   uSeq;            //序号，每一帧本序号加一
        unsigned int   uSegId;          //段ID，确保全局唯一或者其代表的flv头是唯一的
        unsigned int   uCheck;          //校验和，本结构体后面数据内容的校验和。
    }tagFLVHEAD;

	typedef struct SCdnFlvHead
	{
		unsigned int	m_uiFlvHeaderLen;

		tagFLVHEAD		m_txHead;
		unsigned char	flvHeadBuf[MAX_FLV_HEAD_LEN];
		
		SCdnFlvHead()
		{
			memset(this, 0, sizeof(SCdnFlvHead));
		}
	}SCdnFlvHead;

    typedef struct SCdnBlock
    {
        unsigned int uiDataLen; 
        tagFLVHEAD m_txHead;
        char blockBuf[MAX_BLOCK_LEN]; 

        SCdnBlock()
        {
            memset(this, 0, sizeof(SCdnBlock));
        }
    }SCdnBlock;

    typedef struct SCdnBlockIndex
    {
        unsigned int uiHaveData;
        unsigned int uiBlockSeq;
        unsigned int uiBlockSec;

        SCdnBlockIndex()
        {
            memset(this, 0, sizeof(SCdnBlockIndex));
        }
    }SCdnBlockIndex;

    typedef struct SHttpRequestPara
    {
        unsigned int uiCmd;
        unsigned int uiRoomId;
        unsigned int uiUin;
        unsigned int uiVideoId;
        unsigned int uiFlow;
        uint64_t     uiTsId;

        SHttpRequestPara()
        {
            uiCmd       = 0;
            uiUin	= 0;
            uiRoomId	= 0;
            uiVideoId	= 0;
            uiFlow	= 0;
            uiTsId	= 0;
        }
    }SHttpRequestPara;

    enum AppType
    {
        APP_CDN	          = 1, // default: flv
        APP_HLS_M3U8      = 2,
        APP_HLS_TS        = 3,
        APP_CROSSDOMAIN_XML = 4,

        APP_INVALID = -1,
    };

    enum FLOW_TYPE
    {
        FLOW_TYPE_CDN   = 1,
        FLOW_TYPE_HLS   = 2,
    };


    enum SFrameSeqStatus
    {
        FRAME_COMPLETE              = 0,
        FRAME_NOT_COMPLETE          = 1,
        FRAME_SIZE_ERROR            = 2,
        FRAME_SIZE_INVALID          = 3,
        FRAME_SIZE_DIFFERENT        = 4,
        FRAME_SLICE_REPEAT          = 5,
        FRAME_TIME_OUT              = 6,

        FRAME_UNKNOWN_STATE
    };

    const unsigned int FLV_HEAD_SIZE			= 4096;
    const unsigned int VIDEO_PKG_SIZE           = 512;
    const unsigned int MAX_PKG_NUM_ONE_FRAME    = 1024;
    const unsigned int MAX_FRAME_SZIE           = MAX_PKG_NUM_ONE_FRAME * VIDEO_PKG_SIZE;

    //用于缓存一个视频帧
    typedef struct SFrameVideo
    {
		unsigned int	uiWidth;
		unsigned int    uiHeight;
		unsigned int    uiFps;
		unsigned int    uiBitRate;
		unsigned int	uiVideoId;
        unsigned int    uiFrameSeq;
        unsigned char   ucKeyFrame;
        unsigned int	uiFrameSize;
        unsigned int    uiCurSize;
        unsigned int    uiFrameTime;
		unsigned int	uiOriginalVideoId;
        char            *pFrameData;

        SFrameVideo()
        {
            clear();
        }

        void clear()
        {
            memset(this, 0, sizeof(*this));
        }
    }SFrameVideo;

    //用于缓存一个视频帧的索引信息
    typedef struct SFrameVideoItem
    {
        unsigned int		uiUin;
        unsigned short		usVer;
        unsigned char		ucHaveData;
        unsigned int		uiFrameSeq;
        unsigned char		ucKeyFrame;
        unsigned int		uiFrameSize;
        unsigned int		uiCurSize;
        unsigned int		uiFrameTime;
        unsigned int		uiStart;
        unsigned int		uiFps;
        unsigned long long	ullTouchTime;
        char				cBeSend;

        SFrameVideoItem()
        {
            clear();
        }

        void clear()
        {
            memset(this, 0, sizeof(*this));
        }

    }SFrameVideoItem;

    typedef struct SFrameCache
    {
	unsigned int uiFrameSeq;
	unsigned int uiFrameLen;
	unsigned char ucKeyFrame;
	unsigned int uiFrametime;
	unsigned char pData[MAX_FRAME_SZIE];

	SFrameCache()
	{	
		memset(this, 0 ,sizeof(SFrameCache));
	}

    }SFrameCache;

    typedef struct SConnInfo
    {
        unsigned int uiFlow;
        unsigned int uiIp;
        unsigned short usPort;
        unsigned int uiSrcType;
        unsigned int uiHelloTime;

        SConnInfo()
        {
            clear();
        }

        void clear()
        {
            memset(this, 0, sizeof(SConnInfo));
        }
    }SConnInfo;

    //videomgr用了SUserInfo结构，新增字段需要放在后面，保持一致
    typedef struct SUserInfo
    {
        unsigned int    uiFlow;               
        unsigned int    uiIp;
        unsigned short  usPort;
        unsigned int    uiSrcType;
        unsigned int    uiHelloTime;

        unsigned int    uiVideoId;
		unsigned int	uiOriginalVideoId;
        
        unsigned int    uiRoomId;
		unsigned int    uiMainRoomId;
        unsigned char   ucRoomType;

        unsigned short  usVer;
		unsigned short  usCmd;
		unsigned short  usSeq;
		unsigned int    uiUin;

		unsigned char	ucSubCmd;

        SUserInfo()
        {
           clear();
        }

        void clear()
        {
            memset(this, 0, sizeof(SUserInfo));
        }

        void set_conn(const SConnInfo& oConnInfo)
        {
            uiFlow		= oConnInfo.uiFlow;
            uiIp		= oConnInfo.uiIp;
            usPort		= oConnInfo.usPort;
            uiSrcType	= oConnInfo.uiSrcType;
            uiHelloTime = oConnInfo.uiHelloTime;
        }
    }SUserInfo;

    typedef struct SFlow2VideoIdInfo
    {
        unsigned int uiFlow;
        unsigned char ucFlowType;
        unsigned int uiVideoId;

        SFlow2VideoIdInfo()
        {
           clear();
        }

        void clear()
        {
            memset(this, 0, sizeof(SFlow2VideoIdInfo));
        }

        bool get_func(const SFlow2VideoIdInfo& lsh, const SFlow2VideoIdInfo& rsh)
        {
            if (!val_fun(lsh))
            {
                return false;
            }

            if (lsh.uiFlow == rsh.uiFlow)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool ins_func(const SFlow2VideoIdInfo& lsh)
        {
            if (lsh.uiFlow == 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool val_fun(const SFlow2VideoIdInfo& lsh)
        {
            if (lsh.uiFlow != 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }SFlow2VideoIdInfo;

    typedef struct SFlowInfo
    {
        unsigned int    uiFlow;               
        unsigned int    uiIp;
        unsigned short  usPort;
        unsigned int    uiSrcType;
        unsigned char   ucFlowType;
	unsigned int	uiSendTime;

        union
        {
            unsigned char ucIsHaveMediaInfo;
        }unionFlag;

        SFlowInfo()
        {
            clear();
        }

        void clear()
        {
            memset(this, 0, sizeof(SFlowInfo));
        }

        void set_conn(const SConnInfo& oConnInfo)
        {
            uiFlow      = oConnInfo.uiFlow;
            uiIp        = oConnInfo.uiIp;
            usPort      = oConnInfo.usPort;
            uiSrcType   = oConnInfo.uiSrcType;
        }

        bool equal(const SFlowInfo &oFlowInfo)
        {
            return uiFlow == oFlowInfo.uiFlow && uiSrcType == oFlowInfo.uiSrcType;
        }

        void clone(const SFlowInfo &oCdnFlowInfo)
        {
            memcpy((void *)this, (void *)&oCdnFlowInfo, sizeof(SFlowInfo));
        }

    }SFlowInfo;

    typedef struct SVideoIdInfo
    {
        unsigned int    uiVideoId;
        unsigned int    uiUin;
        unsigned int    uiRoomId;
	unsigned int    uiMainRoomId;
	unsigned int	uiChannelId;
        unsigned char   ucRoomType;
        unsigned int    uiTouchTime;

        SFlowInfo    oCdnFlowList[MAX_CDN_FLOW_NUM];

        SVideoIdInfo()
        {
            clear();
        }

        void clear()
        {
            memset(this, 0, sizeof(SVideoIdInfo));
        }

	void reSetFlvSendFlag()
	{
		for(unsigned int i = 0; i < MAX_CDN_FLOW_NUM; i++)
		{
			oCdnFlowList[i].unionFlag.ucIsHaveMediaInfo = 0;
		}
	}

        bool get_func(const SVideoIdInfo& lsh, const SVideoIdInfo& rsh)
        {
            if (!val_fun(lsh))
            {
                return false;
            }

            if (lsh.uiVideoId == rsh.uiVideoId)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool ins_func(const SVideoIdInfo& lsh)
        {
            if (lsh.uiVideoId == 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool val_fun(const SVideoIdInfo& lsh)
        {
            if (lsh.uiVideoId != 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }SVideoIdInfo;

    const unsigned int MAX_VOICESVR_NUM = 128;

    typedef struct SMediaInfo
    {
        unsigned int  m_uiUin;
        unsigned char m_ucLen;
	unsigned char m_ucResLen;
        unsigned char m_ucVideoType;
        unsigned int  m_uiWidth;
        unsigned int  m_uiHeight;
        unsigned int  m_uiBitRate;
        unsigned int  m_uiVideoId;
        unsigned int  m_uiFps;
        unsigned char m_buff[FLV_HEAD_SIZE];   
		

        SMediaInfo()
        {
		clear();
        }

	void clear()
	{
		memset(this,0,sizeof(SMediaInfo));
	}

        void clone(const SMediaInfo &oMediaInfo)
        {
            memcpy((char *)this, (char *)&oMediaInfo, sizeof(*this));
        }

        string to_string(const string &tab) const 
        {
            stringstream ss;
            ss << tab << "m_uiUin: " << m_uiUin << "\n"
                << tab << "m_ucLen: " << (unsigned int)m_ucLen << "\n" 
                << tab << "m_ucVideoType: " << (unsigned int)m_ucVideoType << "\n"
                << tab << "m_uiWidth: " << m_uiWidth << "\n"
                << tab << "m_uiHeight: " << m_uiHeight << "\n"
                << tab << "m_uiBitRate: " << m_uiBitRate << "\n"
                << tab << "m_uiFps: " << m_uiFps << "\n"
                << tab << "m_buff: [";

            for (unsigned int i=0; i<FLV_HEAD_SIZE; ++i)
            {
                if (i>=m_ucLen)
                {
                    break;
                }  

				if(i % 16 == 0)
				{
					ss << "\n";
				}

                ss << hex << (unsigned int)m_buff[i] << " ";
            }

            ss << "]";

            return ss.str();
        };
    }SMediaInfo;

    typedef struct SCdnMediaInfo
    {
        unsigned char   m_ucLen;
        unsigned short  m_usVer;
        tagFLVHEAD      m_txHead;
        unsigned char   m_buff[FLV_HEAD_SIZE];

		SCdnMediaInfo()
		{
			clear();
		}

		void clear()
		{
			memset(this, 0, sizeof(SCdnMediaInfo));
		}
		
    }SCdnMediaInfo;

    enum SVideoType
    {
        GAME_VIDEO          = 255,
        DESKTOP_VIDEO       = 254,
        CAMERA_VIDEO        = 1,
    };

    typedef struct SVoiceChannelIdInfo
    { 
        unsigned int    uiKey;
        unsigned int    uiChannelId;

        SVoiceChannelIdInfo()
        {
            clear();
        }

        void clear()
        {
            memset(this, 0, sizeof(*this));
        }

        bool get_func(const SVoiceChannelIdInfo& lsh, const SVoiceChannelIdInfo& rsh)
        {
            if (!val_fun(lsh))
            {
                return false;
            }

            if (lsh.uiKey == rsh.uiKey)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool ins_func(const SVoiceChannelIdInfo& lsh)
        {
            if (lsh.uiKey == 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool val_fun(const SVoiceChannelIdInfo& lsh)
        {
            if (lsh.uiKey != 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }SVoiceChannelIdInfo;

	typedef struct SRoom2VideoId  
	{
		unsigned int uiRoomId;
		unsigned int uiVideoId;
		unsigned int uiVideoUin;
		unsigned int uiTouchTime;

		SRoom2VideoId()
		{
			memset(this, 0, sizeof(SRoom2VideoId));
		}

		void clear()
		{
			memset(this, 0, sizeof(SRoom2VideoId));
		}

		void updateRoomId2VideoIdInfo(unsigned int uiNewVideoId, unsigned int uiNewVideoUin, unsigned int uiNewTouchTime)
		{
			uiVideoId	= uiNewVideoId;
			uiVideoUin	= uiNewVideoUin;
			uiTouchTime	= uiNewTouchTime;
		}

		bool get_func(const SRoom2VideoId& lsh, const SRoom2VideoId& rsh)
		{
			if (!val_fun(lsh))
			{
				return false;
			}

			if (lsh.uiRoomId == rsh.uiRoomId)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool ins_func(const SRoom2VideoId& lsh)
		{
			if (lsh.uiRoomId == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool val_fun(const SRoom2VideoId& lsh)
		{
			if (lsh.uiRoomId != 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}


	}SRoom2VideoId;
	

const unsigned int MAX_VOICEDATA_SIZE		= 512;
const unsigned int MAX_VOICEDATA_ITEM_NUM	= 4096;

	typedef struct SVoiceItem
	{	
		unsigned int uiSeq;
		unsigned int uiSize;
		unsigned int uiAudioTime;
        unsigned int uiTime;

		unsigned char  cBuf[MAX_VOICEDATA_SIZE];

		SVoiceItem()
		{
			clear();
		}

		void clear()
		{
			memset(this, 0, sizeof(SVoiceItem));
		}

	} SVoiceItem;

	typedef struct ClientHeader
	{
		unsigned int    uiLength;           // 包长度，包括数据字段(包括ClientHeader)
		unsigned int    uiUin;              // 用户UIN
		unsigned short  ushVersion;         // 协议版本号， qt填0
		unsigned int    uiAppID;            // 应用ID，     qt填0
		unsigned int    uiZoneID;           // 大区ID，     qt填0
		unsigned short  ushCmdID;           // 命令字
		unsigned int    uiDestSvrID;        // 目的服务器类型，qt填0
		unsigned int    uiRequest;          // 客户端序列号,   qt填0
		unsigned short  ushCheckSum;        // 数据校验,       qt填0

		ClientHeader()
		{
			memset(this, 0, sizeof(ClientHeader));
		}

	}ClientHeader;

#pragma pack()
}

#endif
