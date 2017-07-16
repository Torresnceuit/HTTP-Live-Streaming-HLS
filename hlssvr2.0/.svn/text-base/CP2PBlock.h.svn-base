#ifndef _CP2PBLOCKINDEXBUFFER_H
#define _CP2PBLOCKINDEXBUFFER_H

#include "CHash.h"
#include "muxsvr.h"

class CP2PBlockIndexBuffer
{
public:
    CP2PBlockIndexBuffer()
    {
        bHaveData = false;
        uiWriteIndex = 0;
        uiBeginBlock = 0;
        uiEndBlock = 0;
        uiBeginSec = 0;
        uiEndSec = 0;
    }

    int add(unsigned int uiBlockSeq, unsigned int uiBlockSec)
    {
        if (false == bHaveData)
        {
            bHaveData = true;

            uiBeginBlock = uiBlockSeq;
            uiBeginSec = uiBlockSec;
        }

        arrSCdnBlockIndex[uiWriteIndex].uiHaveData = 1;
        arrSCdnBlockIndex[uiWriteIndex].uiBlockSeq = uiBlockSeq;
        arrSCdnBlockIndex[uiWriteIndex].uiBlockSec = uiBlockSec;

        uiEndBlock = uiBlockSeq;
        uiEndSec = uiBlockSec;

        if (1 == arrSCdnBlockIndex[(uiWriteIndex+1)%muxsvr::FRAME_ITEM_NUM].uiHaveData)
        {
            uiBeginBlock = arrSCdnBlockIndex[(uiWriteIndex+1)%muxsvr::FRAME_ITEM_NUM].uiBlockSeq;
            uiBeginSec = arrSCdnBlockIndex[(uiWriteIndex+1)%muxsvr::FRAME_ITEM_NUM].uiBlockSec;
        }

        uiWriteIndex = (uiWriteIndex+1)%muxsvr::FRAME_ITEM_NUM;

        return 0;
    }

    bool haveData() const
    {
        return bHaveData;
    }

    unsigned int getBeginBlock() const
    {
        return uiBeginBlock;
    }

    unsigned int getEndBlock() const
    {
        return uiEndBlock;
    }

    unsigned int getBeginSec() const
    {
        return uiBeginSec;
    }

    unsigned int getEndSec() const
    {
        return uiEndSec;
    }

private:
    bool bHaveData;
    unsigned int uiWriteIndex;
    unsigned int uiBeginBlock;
    unsigned int uiEndBlock;
    unsigned int uiBeginSec;
    unsigned int uiEndSec;

    //保存视频帧的索引信息，最多保存FRAME_ITEM_NUM帧
    muxsvr::SCdnBlockIndex arrSCdnBlockIndex[muxsvr::FRAME_ITEM_NUM];
};

#endif
