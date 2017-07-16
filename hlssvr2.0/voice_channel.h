#ifndef _VOICE_CHANNEL_H
#define _VOICE_CHANNEL_H

#include <sys/shm.h>

#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "CHash.h"
#include "CMultiHash.h"

#include "muxsvr.h"
#include "muxsvr_mcd_cfg.h"

#include "CFrameBuffer.h"
#include "CAudioBuffer.h"

#include "flv/flv_parser.h"

#include "tfc_base_comm.h"
#include "CP2PBlock.h"

#include "hls/hls_stream.h"

using std::map;
using std::string;
using std::stringstream;

namespace muxsvr 
{
    class CMCDProc;
    struct SVoiceChannel;

    typedef struct SVoiceChannelIndex 
    {
        unsigned int uiKey;
        SVoiceChannel *pVoiceChannel;
    }SVoiceChannelIndex;

    typedef std::list<SVoiceChannelIdInfo *>   TVoiceChannelIdInfoList;

    //记录一个通道的语音
    typedef struct SVoiceChannel
    {
    public:
        SVoiceChannel()
        {
            m_bCreateTime = false;
        }

        ~SVoiceChannel()
        {
        }

        void clear()
        {
            memset(this, 0, sizeof(*this));
        }

        int open(const SVoiceChannel &oVoiceChannel, unsigned int uiChannelId);

        int pushBackVideoFrame(const SUserInfo &oUserInfo, const SFrameVideo &oFrameVideo, unsigned long long ullTime);
        int pushBackAudioFrame(const SUserInfo &oUserInfo, unsigned int uiSeq, unsigned char *pData, unsigned int uiSize, unsigned int uiTime);

        int setUserInfo(unsigned int uiUin, unsigned int uiRoomId, unsigned char ucRoomType, unsigned int uiVideoId, unsigned short usVer)
        {
            setUin(uiUin);
            setRoomId(uiRoomId);
            setRoomType(ucRoomType);
            setVideoId(uiVideoId);
            setVer(usVer);

            return 0;
        }

        int setCreateTime(unsigned long long ull_now_Tick)
        {
            if (false == m_bCreateTime)
            {
                m_bCreateTime = true;
                m_ullCreateTime = ull_now_Tick;
            }

            return 0;
        }

        int setUin(unsigned int uiUin)
        {
            m_uiUin = uiUin;
            return 0;
        }

        int setRoomId(unsigned int uiRoomId)
        {
            m_uiRoomId = uiRoomId;
            return 0;
        }

        int setRoomType(unsigned char ucRoomType)
        {
            m_ucRoomType = ucRoomType;
            return 0;
        }

        int setVideoId(unsigned int uiVideoId)
        {
            m_uiVideoId = uiVideoId;
            return 0;
        }

        int setVer(unsigned int usVer)
        {
            m_usVer = usVer;
            return 0;
        }

        unsigned int getUin() const
        {
            return m_uiUin;
        }

        unsigned int getRoomId() const
        {
            return m_uiRoomId;
        }

        unsigned char getRoomType() const
        {
            return m_ucRoomType;
        }

        unsigned int getVideoId() const
        {
            return m_uiVideoId;
        }

        unsigned short getVer() const
        {
            return m_usVer;
        }

        unsigned int getPushTime()
        {
            return m_uiPushTime;
        }

        int getFrameBuffer(const SFrameVideoItem **ppFrameVideoItemBuffer, 
                   unsigned int *puiFrameVideoItemBufferSize, const char **pFrameDataBuff, 
                   unsigned int *puiFrameDataBuffSize)
        {
            int ret = m_oFrameBuffer.get_frame_buffer(
                ppFrameVideoItemBuffer,
                puiFrameVideoItemBufferSize,
                pFrameDataBuff,
                puiFrameDataBuffSize);

            return ret;
        }

        int getFrameBufferInOrder(const SFrameVideoItem **ppFrameVideoItemBuffer, 
                     const char **pFrameDataBuff,unsigned long long ullTime)
        {
            return m_oFrameBuffer.get_frame_buffer_in_order(ppFrameVideoItemBuffer,pFrameDataBuff,ullTime);
        }

        int getAudioFrameInOrder(unsigned int &uiSeq, unsigned char **pData, 
                     unsigned int &uiSize, unsigned int &uiAudioTime, unsigned int uiTime)
        {
            return m_oAudioBuffer.get_voice_in_order(uiSeq,pData,uiSize,uiAudioTime,uiTime);
        }

        unsigned int getChannelId() const
        {
            return m_uiChannelId;
        }

        /*unsigned int getReceiveFrameNum() const
        {
        return m_oFrameBuffer.getReceiveFrameNum();
        }

        unsigned int getCompleteFrameNum() const
        {
        return m_oFrameBuffer.getCompleteFrameNum();
        }

        unsigned int getOverTimeFrameNum() const
        {
        return m_oFrameBuffer.getOverTimeFrameNum();
        }

        unsigned int getCircleLen() const
        {
        return m_oFrameBuffer.getCircleLen();
        }

        unsigned int getMaxCircleLen() const
        {
        return m_oFrameBuffer.getMaxCircleLen();
        }

        unsigned int getMinCircleLen() const
        {
        return m_oFrameBuffer.getMinCircleLen();
        }

        unsigned int getMaxFrameSize() const
        {
        return m_oFrameBuffer.getMaxFrameSize();
        }*/

        string t2s(const time_t t,const string& format="%Y-%m-%d %H:%M:%S")
        {
            struct tm curr;
            curr = *localtime(&t);
            char sTmp[1024];
            strftime(sTmp,sizeof(sTmp),format.c_str(),&curr);
            return string(sTmp);
        }

        string to_string(const string &tab) 
        {
            stringstream ss;

            ss << tab << "uiChannelId:" << m_uiChannelId << "\n"
                << tab << "uiUin:" << m_uiUin << "\n"
                << tab << "uiRoomId:" << m_uiRoomId << "\n"
                << tab << "ucRoomType: " << m_ucRoomType << "\n"
                << tab << "uiVideoId: " << m_uiVideoId << "\n"
                << tab << "usVer: " << m_usVer << "\n"

                << tab << "ullCreateTime: " << m_ullCreateTime << "\n"
                << tab << "uiPushTime: " << t2s((const time_t)m_uiPushTime).c_str() << "\n"
                << tab << "fAverageDis: " << m_oFrameBuffer.getAverageRWDistance() << "\n"
                << tab << "audio_dis: " << abs(int(m_oAudioBuffer.getReadSeq() - m_oAudioBuffer.getWriteSeq())) 
                <<" read: " << m_oAudioBuffer.getReadSeq() << " write: " << m_oAudioBuffer.getWriteSeq() << "\n"
                << tab << "video_dis: " << abs(int(m_oFrameBuffer.getReadSeq() - m_oFrameBuffer.getWriteSeq())) 
                <<" read: " << m_oFrameBuffer.getReadSeq() << " write: " << m_oFrameBuffer.getWriteSeq() << "\n"
                ;

            return ss.str();
        };

        CFrameBuffer* getCFrameBuffer()
        {
            return &m_oFrameBuffer;
        }

        CAudioBuffer* getCAudioBuffer()
        {
            return &m_oAudioBuffer;
        }

        unsigned int getVideoReadSeq() const
        {
            return m_oFrameBuffer.getReadSeq();
        }

        unsigned int getVideoWriteSeq() const
        {
            return m_oFrameBuffer.getWriteSeq();
        }

        unsigned int getAudioReadSeq() const
        {
            return m_oAudioBuffer.getReadSeq();
        }

        unsigned int getAudioWriteSeq() const
        {
            return m_oAudioBuffer.getWriteSeq();
        }

        float getAverageRWDistance() const
        {
            return m_oFrameBuffer.getAverageRWDistance();
        }

        unsigned int getRWDistance() const
        {
            return m_oFrameBuffer.getRWDistance();
        }

    public:

        SCdnFlvHead	    m_cdnFlvHead;
        SFlvParser	    m_flv_parser;

	bool			m_bAudioChanged;
	unsigned char	adtsheader[7];
		
        //保存通道的视频帧数据
        CFrameBuffer m_oFrameBuffer;
        CAudioBuffer m_oAudioBuffer;
        CP2PBlockIndexBuffer m_oP2PBlockIndexBuffer;
        SCdnBlock   m_oCdnBlock;
        time_t		 m_LastTickVideoIdUpdateTime;

        unsigned int m_uiCdnSeqNum;

    private:
        bool m_bCreateTime;
        unsigned long long m_ullCreateTime;


    private:
        //视频包统计信息
        unsigned int m_uiPushTime;
        unsigned int m_uiChannelId;

    private:
        //存视频主播信息
        unsigned int m_uiUin;
        unsigned int m_uiRoomId;
        unsigned int m_ucRoomType;
        unsigned int m_uiVideoId;

        unsigned short m_usVer;

    public:
        hls_session_t *hls_session;

    private:
        unsigned char   reserve[64];
    }SVoiceChannel;

    const unsigned int INVALID_VOICECHANNEL_ID = 0xffffffff;

    class CDataChannelMgr
    {
        friend class CMonitor;

    public:
        CDataChannelMgr(CMCDProc *proc)
            :m_pVoiceChannelIdMgr(NULL)
            ,m_pVoiceChannelAllocMgr(NULL)
            ,m_pMCD(proc)
        {
            m_pVoiceChannelIdMgr = new CVoiceChannelIdMgr(proc);
            m_pVoiceChannelAllocMgr = new CVoiceChannelAllocMgr(proc);
        }

        ~CDataChannelMgr()
        {
            if (m_pVoiceChannelIdMgr != NULL)
            {
                delete m_pVoiceChannelIdMgr;
            }

            if (m_pVoiceChannelAllocMgr != NULL)
            {
                delete m_pVoiceChannelAllocMgr;
            }

            m_pVoiceChannelIdMgr = NULL;
            m_pVoiceChannelAllocMgr = NULL;
            m_pMCD = NULL;
        }

        int init()
        {
            if (m_pVoiceChannelIdMgr == NULL || m_pVoiceChannelAllocMgr == NULL)
            {
                return -1;
            }

            int ret = 0;

            ret = m_pVoiceChannelIdMgr->init();

            if (ret < 0)
            {
                return ret;
            }

            ret = m_pVoiceChannelAllocMgr->init();

            if (ret < 0)
            {
                return ret;
            }

            return 0;
        }

        SVoiceChannel *get_voice_channel(unsigned int uiKey);
        SVoiceChannel *allocate_voice_channel(unsigned int uiKey, bool &bIsNew);
        int release_voice_channel(unsigned int uiKey);

        int val(TVoiceChannelIdInfoList &oList)
        {
            return m_pVoiceChannelIdMgr->val(oList);
        }

        unsigned int getFreeChannelNum() const
        {
            return m_pVoiceChannelAllocMgr->freeChannelNum();
        }

        bool DoCleanInvalidVoiceChannel();

        int get_used_voice_channel(std::vector<SVoiceChannel *> &vecActiveChannel)
        {
            return m_pVoiceChannelAllocMgr->get_used_voice_channel(vecActiveChannel);
        }

    private:
        //key -> channelId
        class CVoiceChannelIdMgr
        {
            typedef CMultiHash<SVoiceChannelIdInfo> THash;

        public:
            CVoiceChannelIdMgr(CMCDProc* proc)
                :m_oHash(m_oVoiceChannelIdInfo, THash::GETFUNC(&SVoiceChannelIdInfo::get_func), THash::INSFUNC(&SVoiceChannelIdInfo::ins_func), THash::INSFUNC(&SVoiceChannelIdInfo::val_fun))
                ,m_pMCD(proc)
            {

            }

            ~CVoiceChannelIdMgr(){}

            SVoiceChannelIdInfo* ins(const SVoiceChannelIdInfo &oVoiceChannelIdInfo);

            SVoiceChannelIdInfo* get(unsigned int uiKey)
            {
                m_oVoiceChannelIdInfo.uiKey = uiKey;
                return m_oHash.get(uiKey, m_oVoiceChannelIdInfo);
            }

            int del(unsigned int uiKey)
            {
                m_oVoiceChannelIdInfo.uiKey = uiKey;
                return m_oHash.del(uiKey, m_oVoiceChannelIdInfo);
            }

            int init();

            int init(unsigned int uiHashKey, const unsigned int uiHashLen, const unsigned int uiHashTime, const bool bCreate)
            {
                int ret = m_oHash.init(uiHashKey, uiHashLen, uiHashTime, bCreate);

                if (ret < 0)
                {
                    return ret;
                }

                m_oHash.clear();

                return 0;
            };

            int val(TVoiceChannelIdInfoList& oList)
            {
                return m_oHash.val(oList);
            }

        private:
            SVoiceChannelIdInfo m_oVoiceChannelIdInfo;
            THash               m_oHash; 
            CMCDProc            *m_pMCD;
        };

        class CVoiceChannelAllocMgr
        {
            friend class CDataChannelMgr;
            friend class CMonitor;
        public:
            CVoiceChannelAllocMgr(CMCDProc* proc):m_pMCD(proc)
            {
                free();
            }

            ~CVoiceChannelAllocMgr()
            {
                free();
                m_pMCD = NULL;
            }

            void free()
            {
                m_puiNodeNum = NULL;
                m_uiFreeListHead = NULL;
                m_uiFreeListTail = NULL;
                m_pIndex = NULL;
                m_pData = NULL;
            }

            int init();

            int init(unsigned int uiKey, unsigned int uiNodeNum, bool bCreate)
            {
                unsigned int uiMemSize = sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(CacheIndex)*uiNodeNum+sizeof(SVoiceChannel)*uiNodeNum;
                int nFlag = 0666;

                int shmId = 0;
                bool bNewCreate = false;
                int shmAddr = 0;

                shmId = shmget(uiKey, uiMemSize, nFlag);

                if (shmId == -1)
                {
                    if(bCreate)
                    {
                        nFlag = nFlag|IPC_CREAT;
                    }
                    else
                    {
                        return -1;
                    }

                    shmId = shmget(uiKey, uiMemSize, nFlag);

                    if (shmId == -1)
                    {
                        return -1;
                    }

                    bNewCreate = true;
                }

                if ((shmAddr = (int)shmat(shmId, NULL, 0)) == -1)            
                {
                    return -1;
                }

                char *pMem = (char *)shmAddr;
                m_puiNodeNum = (unsigned int *)pMem;
                m_uiFreeListHead = (unsigned int *)(pMem+sizeof(unsigned int));
                m_uiFreeListTail = (unsigned int *)(pMem+sizeof(unsigned int)+sizeof(unsigned int));
                m_uiUsedListHead = (unsigned int *)(pMem+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int));
                m_uiUsedListTail = (unsigned int *)(pMem+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int));
                m_pIndex = (CacheIndex *)(pMem+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int));
                m_pData = (SVoiceChannel *)(pMem+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(CacheIndex)*uiNodeNum);

                //if (bNewCreate)
                //{
                *m_puiNodeNum = uiNodeNum;
                *m_uiFreeListHead = INVALID_VOICECHANNEL_ID;
                *m_uiFreeListTail = INVALID_VOICECHANNEL_ID;
                *m_uiUsedListHead = INVALID_VOICECHANNEL_ID;
                *m_uiUsedListTail = INVALID_VOICECHANNEL_ID;
                memset((char *)m_pIndex, 0, sizeof(CacheIndex)*uiNodeNum);
                memset((char *)m_pData, 0, sizeof(SVoiceChannel)*uiNodeNum);

                for (unsigned int i=0; i<*m_puiNodeNum; i++)
                {
                    m_pData[i].clear();
                    m_pIndex[i].clear();
                    m_pIndex[i].uiPrev = INVALID_VOICECHANNEL_ID;
                    m_pIndex[i].uiNext = INVALID_VOICECHANNEL_ID;

                    if (INVALID_VOICECHANNEL_ID != *m_uiFreeListTail)
                    {
                        m_pIndex[*m_uiFreeListTail].uiNext = i;//空闲队列非空，添加到队列尾部
                        m_pIndex[i].uiPrev = *m_uiFreeListTail;//空闲队列非空，添加到队列尾部
                    }

                    *m_uiFreeListTail = i;//设置空闲队列尾部

                    if (INVALID_VOICECHANNEL_ID == *m_uiFreeListHead)
                    {
                        *m_uiFreeListHead = i;//设置空闲队列头部
                    }
                }
                //}

                SUBMCD_DEBUG_P(LOG_NORMAL, "CVoiceChannelAllocMgr::init pMem: 0x%x\n", (unsigned int)pMem);

                return 0;
            };

            unsigned int allocate_voice_channel()
            {
                if (INVALID_VOICECHANNEL_ID != *m_uiFreeListHead) 
                { 
                    unsigned int uiChannelId = *m_uiFreeListHead;
                    *m_uiFreeListHead = m_pIndex[uiChannelId].uiNext;//空闲队列头元素后移一位，得到新空闲头元素
                    
                    if (INVALID_VOICECHANNEL_ID == *m_uiFreeListHead)
                    {
                        *m_uiFreeListTail = INVALID_VOICECHANNEL_ID;//新空闲头元素设置为INVALID_VOICECHANNEL_ID
                    }
                    else
                    {
                        m_pIndex[*m_uiFreeListHead].uiPrev = INVALID_VOICECHANNEL_ID;//新空闲头元素前向指针设置为INVALID_VOICECHANNEL_ID
                    }

                    if (INVALID_VOICECHANNEL_ID != *m_uiUsedListTail)
                    {
                        m_pIndex[*m_uiUsedListTail].uiNext = uiChannelId;//已分配队列非空，添加到队列尾部
                    }

                    m_pIndex[uiChannelId].uiPrev = *m_uiUsedListTail;//添加到已分配队列尾部
                    m_pIndex[uiChannelId].uiNext = INVALID_VOICECHANNEL_ID;//添加到已分配队列尾部
                    *m_uiUsedListTail = uiChannelId;//添加到已分配队列尾部

                    if (INVALID_VOICECHANNEL_ID == *m_uiUsedListHead)
                    {
                        *m_uiUsedListHead = uiChannelId;//设置已分配队列头部
                    }
                    
                    m_pData[uiChannelId].open(m_oVoiceChannel, uiChannelId);

                    SUBMCD_DEBUG_P(LOG_NORMAL, "CVoiceChannelAllocMgr::allocate_voice_channel: %u\n", uiChannelId);

                    return uiChannelId;
                }

                return INVALID_VOICECHANNEL_ID;
            }

            int release_voice_channel(unsigned int uiChannelId)
            {
                //在调用release_voice_channel函数的情况下，m_uiUsedListHead和m_uiUsedListTail肯定非空

                if (*m_uiUsedListHead == uiChannelId)
                    //释放的通道为已使用队列的头部
                {
                    *m_uiUsedListHead = m_pIndex[uiChannelId].uiNext;

                    if (INVALID_VOICECHANNEL_ID != *m_uiUsedListHead)
                    {
                        m_pIndex[*m_uiUsedListHead].uiPrev = INVALID_VOICECHANNEL_ID;
                    }
                    else
                    {
                        *m_uiUsedListTail = INVALID_VOICECHANNEL_ID;
                    }
                }
                else if (*m_uiUsedListTail == uiChannelId)
                    //释放的通道非已使用队列的头部，为已使用队列的尾部
                {
                    *m_uiUsedListTail = m_pIndex[uiChannelId].uiPrev;//*m_uiUsedListTail肯定非空
                    m_pIndex[*m_uiUsedListTail].uiNext = INVALID_VOICECHANNEL_ID;
                }
                else
                {
                    m_pIndex[m_pIndex[uiChannelId].uiPrev].uiNext = m_pIndex[uiChannelId].uiNext;
                    m_pIndex[m_pIndex[uiChannelId].uiNext].uiPrev = m_pIndex[uiChannelId].uiPrev;
                }

                m_pData[uiChannelId].clear();
                m_pIndex[uiChannelId].clear();
                m_pIndex[uiChannelId].uiPrev = INVALID_VOICECHANNEL_ID;
                m_pIndex[uiChannelId].uiNext = INVALID_VOICECHANNEL_ID;

                if (INVALID_VOICECHANNEL_ID != *m_uiFreeListTail)
                {
                    m_pIndex[*m_uiFreeListTail].uiNext = uiChannelId;//空闲队列非空，添加到队列尾部
                    m_pIndex[uiChannelId].uiPrev = *m_uiFreeListTail;//空闲队列非空，添加到队列尾部
                }

                *m_uiFreeListTail = uiChannelId;//设置空闲队列尾部

                if (INVALID_VOICECHANNEL_ID == *m_uiFreeListHead)
                {
                    *m_uiFreeListHead = uiChannelId;//设置空闲队列头部
                }

                return 0;
            }

            SVoiceChannel *get_voice_channel(unsigned int uiChannelId)
            {
                return &m_pData[uiChannelId];
            }

            unsigned int freeChannelNum() const
            {
                unsigned int uiNum = 0;
                unsigned int pIndex = *m_uiFreeListHead;

                for (unsigned int i=0; i<*m_puiNodeNum; i++)
                {
                    if(INVALID_VOICECHANNEL_ID == pIndex)
                    {
                        break;
                    }

                    uiNum++;
                    pIndex = m_pIndex[pIndex].uiNext;
                }

                return uiNum;
            }

            int get_used_voice_channel(std::vector<SVoiceChannel *> &vecActiveChannel)
            {
                unsigned int pIndex = *m_uiUsedListHead;

                for (unsigned int i=0; i<*m_puiNodeNum; i++)
                {
                    if(INVALID_VOICECHANNEL_ID == pIndex)
                    {
                        break;
                    }

                    vecActiveChannel.push_back(&m_pData[pIndex]);
                    pIndex = m_pIndex[pIndex].uiNext;
                }
                
                return 0;
            }

        private:
            typedef struct CacheIndex
            {
                unsigned int uiPrev;
                unsigned int uiNext;

                CacheIndex()
                {
                    clear();
                }

                void clear()
                {
                    memset(this, 0, sizeof(CacheIndex));
                }

            }CacheIndex;

        private:
            unsigned int *m_puiNodeNum;
            unsigned int *m_uiFreeListHead;
            unsigned int *m_uiFreeListTail;
            unsigned int *m_uiUsedListHead;
            unsigned int *m_uiUsedListTail;
            CacheIndex *m_pIndex;
            SVoiceChannel *m_pData;

        private:
            SVoiceChannel m_oVoiceChannel;

        private:
            CMCDProc *m_pMCD;
        };

    private:
        CVoiceChannelIdMgr      *m_pVoiceChannelIdMgr;
        CVoiceChannelAllocMgr   *m_pVoiceChannelAllocMgr;

    private:
        CMCDProc *m_pMCD;
    };

}

#endif
