#ifndef _AUDIO_BUFFER_H
#define _AUDIO_BUFFER_H

#include "tfc_debug_log.h"
#include "submcd_log.h"

#include "muxsvr.h"

using namespace muxsvr;

class CAudioBuffer
{
    enum
    {
        MAX_JUMP_NUM = 50,
        MAX_TIMEOUT  = 1,
    };

	public:
		CAudioBuffer()
			:m_uiWriteSeq(0)
			,m_uiReadSeq(0)
		{}

		~CAudioBuffer(){};

		int push_back(const SUserInfo &oUserInfo, unsigned int uiSeq, unsigned char *pData, 
                              unsigned int uiSize, unsigned int uiTime)
		{
			if(pData == NULL || uiSize < 22 || uiSize > MAX_VOICEDATA_SIZE)
			{
				return -1;
			}

                        unsigned int uiDistance		= abs((int)(uiSeq - m_uiWriteSeq));
			unsigned int uiRWDistance	= abs((int)(m_uiWriteSeq - m_uiReadSeq));

                        if(m_uiWriteSeq !=0 && (uiDistance > MAX_JUMP_NUM || uiRWDistance > 384))
                        {
				SUBMCD_DEBUG_P(LOG_DEBUG,"CAudioBuffer::push_back reset data "
                                      "uin: %u, roomid: %u, videoid: %u, audio_seq: %u, read: %u, write: %u\n",
					oUserInfo.uiUin,
					oUserInfo.uiRoomId,
					oUserInfo.uiVideoId,
					uiSeq,
					m_uiReadSeq,
					m_uiWriteSeq);

                                clearData();
                         }
	
			unsigned int uiIndex = uiSeq % MAX_VOICEDATA_ITEM_NUM;

			memcpy(m_oVoiceItems[uiIndex].cBuf, pData, uiSize);

			m_oVoiceItems[uiIndex].uiSeq		= uiSeq;
			m_oVoiceItems[uiIndex].uiSize		= uiSize;
			m_oVoiceItems[uiIndex].uiAudioTime	= ntohl(*(unsigned int*)(pData + uiSize - 5));
                        m_oVoiceItems[uiIndex].uiTime		= uiTime;

			m_uiWriteSeq = uiSeq;

			return 0;
		}

		int get_voice_in_order(unsigned int &uiSeq, unsigned char **pData, unsigned int &uiSize, 
                                       unsigned int &uiAudioTime, unsigned int uiTime)
		{
                        if(0 == m_uiReadSeq)
                        {
                            m_uiReadSeq = m_uiWriteSeq;
                        }

                        if(m_uiReadSeq >= m_uiWriteSeq)
                        {
                            return -1;
                        }

			unsigned int uiIndex = m_uiReadSeq % MAX_VOICEDATA_ITEM_NUM;

                        if(m_oVoiceItems[uiIndex].uiSeq != m_uiReadSeq)
                        {
				if(0 == m_oVoiceItems[uiIndex].uiTime)
				{
					m_oVoiceItems[uiIndex].uiTime = uiTime;
				}

				if(uiTime - m_oVoiceItems[uiIndex].uiTime > MAX_TIMEOUT) 
				{
					m_uiReadSeq++;		
				}

				return -3;
                        }
            
			*pData      = m_oVoiceItems[uiIndex].cBuf;
                        uiSize      = m_oVoiceItems[uiIndex].uiSize;
                        uiAudioTime = m_oVoiceItems[uiIndex].uiAudioTime;
                        uiSeq       = m_oVoiceItems[uiIndex].uiSeq;  

			m_oVoiceItems[uiIndex].uiTime = 0;

                        m_uiReadSeq++;

			return 0;
		}

        void clearData()
        {
            m_uiWriteSeq    = 0;
            m_uiReadSeq     = 0;

            memset(m_oVoiceItems,0,sizeof(m_oVoiceItems));
        }

		unsigned int getReadSeq() const
		{
			return m_uiReadSeq;
		}

		unsigned int getWriteSeq() const
		{
			return m_uiWriteSeq;
		}

    private:
		unsigned int m_uiWriteSeq;
		unsigned int m_uiReadSeq;

	private:
		SVoiceItem m_oVoiceItems[MAX_VOICEDATA_ITEM_NUM];
};
#endif
