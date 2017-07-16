#include "buffer.h"

CVBuffer::CVBuffer(void) : 
m_Buffer(m_Buf),
    m_capacity(sizeof(m_Buf))
{
    Clear();
}

CVBuffer::CVBuffer(const char* pBuffer,const unsigned int uSize) :
m_Buffer(m_Buf),
    m_capacity(sizeof(m_Buf))
{
    Clear();
    setBuffer(pBuffer,uSize);
}

CVBuffer::~CVBuffer(void)
{
}

bool CVBuffer::setBuffer(const char* pBuffer, const unsigned int uSize)
{
    if( pBuffer == NULL || uSize == 0)return false;

    if(uSize>m_capacity)return false;

    memcpy(m_Buffer,pBuffer,uSize);

    m_uOffset=uSize;
    m_uGetOffset = 0;

    return true;
}

bool CVBuffer::setSize(unsigned int uSize)
{
    m_uOffset=uSize;
    m_uGetOffset = 0;

    return true;
}

bool CVBuffer::Append(const char* pData, const unsigned int uLength)
{
    if(pData == NULL || uLength == 0)return false;

    unsigned int uSize = uLength;

    if(m_uOffset+uSize>m_capacity)return false;

    memcpy(m_Buffer+m_uOffset,pData,uSize);

    m_uOffset+=uSize;

    return true;
}

bool CVBuffer::Get(char* pData, const unsigned int uLength)
{
    if( pData == NULL || uLength == 0)return false;

    unsigned int uSize = uLength;

    if(m_uGetOffset+uSize>m_uOffset)return false;

    memcpy(pData,m_Buffer+m_uGetOffset,uSize);

    m_uGetOffset+=uSize;

    return true;
}

bool CVBuffer::Copy(char* pData,unsigned int uLength)
{
    if( pData == NULL || uLength == 0)return false;

    unsigned int uSize = uLength;

    if(m_uGetOffset+uSize>m_uOffset)return false;

    memcpy(pData,m_Buffer+m_uGetOffset,uSize);

    return true;
}

const unsigned int CVBuffer::getSize() const 
{
    return m_uOffset;
}

const char* CVBuffer::getBuffer() const
{
    return m_Buffer;
}

char* CVBuffer::getBegin()
{
    return m_Buffer;
}

void CVBuffer::Clear()
{
    m_uOffset=0;
    m_uGetOffset = 0;

	//提高效率
    memset(m_Buffer,0,m_capacity);
}
